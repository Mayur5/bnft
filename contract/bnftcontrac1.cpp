#include "bnftcontrac1.hpp"
using namespace eosio;

ACTION bnft::create( name issuer, std::string sym, std::string scheme_name, std::string scheme_desc, std::string scheme_date ) {

    require_auth( _self );

    // Check if issuer account exists
    eosio_assert( is_account( issuer ), "issuer account does not exist");

    // Valid symbol
    asset supply(0, symbol( symbol_code( sym.c_str() ), 0) );

    auto symbol = supply.symbol;
    eosio_assert( symbol.is_valid(), "invalid symbol name" );

    // Check if currency with symbol already exists
    auto symbol_name = symbol.code().raw();
    currency_index currency_table( _self, symbol_name );
    auto existing_currency = currency_table.find( symbol_name );
    eosio_assert( existing_currency == currency_table.end(), "token with symbol already exists" );

    // Create new currency
    currency_table.emplace( _self, [&]( auto& currency ) {
        currency.supply = supply;
        currency.issuer = issuer;
        currency.schemeName = scheme_name;
        currency.schemeDesc = scheme_desc;
        currency.schemeDate = scheme_date;
    });
}

ACTION bnft::issue( name to,
                   asset quantity,
                   vector<uint64_t> uris,
                   string tkn_name,
                   string memo) {

    eosio_assert( is_account( to ), "to account does not exist");

    // e,g, Get EOS from 3 EOS
    auto symbol = quantity.symbol;
    eosio_assert( symbol.is_valid(), "invalid symbol name" );
    eosio_assert( symbol.precision() == 0, "quantity must be a whole number" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    eosio_assert( tkn_name.size() <= 32, "name has more than 32 bytes" );

    // Ensure currency has been created
    auto symbol_name = symbol.code().raw();
    currency_index currency_table( _self, symbol_name );
    auto existing_currency = currency_table.find( symbol_name );
    eosio_assert( existing_currency != currency_table.end(), "token with symbol does not exist. create token before issue" );
    const auto& st = *existing_currency;

    // Ensure have issuer authorization and valid quantity
    //require_auth( st.issuer );
    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must issue positive quantity of NFT" );
    eosio_assert( symbol == st.supply.symbol, "symbol precision mismatch" );

    // Increase supply
    add_supply( quantity );

    // Check that number of tokens matches uri size
    eosio_assert( quantity.amount == uris.size(), "mismatch between number of tokens and uris provided" );

    // Mint nfts
    mapping_index map( _self, symbol_name );
    auto existing_aadhaar = map.get_index<"byaadhaar"_n>();
    for(auto const& uri: uris) {
        auto itr = existing_aadhaar.find(uri);
        //eosio_assert(itr == existing_aadhaar.end(), "BNFT already issued");
        if(itr == existing_aadhaar.end()) {
            id_type tokenId = mint( to, asset{1, symbol}, uri, tkn_name);
            map.emplace( to, [&]( auto& row ) {
                //row.id = map.available_primary_key();
                row.id = tokenId;
                row.sym = quantity;
                row.aadhaar = uri;
                row.status = 1;
            });
        } else {
            send_summary(to, std::to_string(uri) + " already issued");
        }
    }

    // Add balance to account
    add_balance( to, quantity, to );
}


ACTION bnft::transferid( name	from,
                        name 	to,
                        id_type	id,
                        string	memo ) {
    // Ensure authorized to send from account
    eosio_assert( from != to, "cannot transfer to self" );
    require_auth( from );

    // Ensure 'to' account exists
    eosio_assert( is_account( to ), "to account does not exist");

    // Check memo size and print
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    // Ensure token ID exists
    auto send_token = tokens.find( id );
    eosio_assert( send_token != tokens.end(), "token with specified ID does not exist" );

    // Ensure owner owns token
    eosio_assert( send_token->owner == from, "sender does not own token with specified ID");

    const auto& st = *send_token;

    // Notify both recipients
    require_recipient( from );
    require_recipient( to );

    // Transfer NFT from sender to receiver
    tokens.modify( send_token, from, [&]( auto& token ) {
        token.owner = to;
    });

    // Change balance of both accounts
    sub_balance( from, st.value );
    add_balance( to, st.value, from );
}

ACTION bnft::transfer( name 	from,
                      name 	to,
                      asset	quantity,
                      string	memo ) {
    // Ensure authorized to send from account
    eosio_assert( from != to, "cannot transfer to self" );
    require_auth( from );

    // Ensure 'to' account exists
    eosio_assert( is_account( to ), "to account does not exist");

    // Check memo size and print
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    eosio_assert( quantity.amount == 1, "cannot transfer quantity, not equal to 1" );

    auto symbl = tokens.get_index<"bysymbol"_n>();

    auto it = symbl.lower_bound(quantity.symbol.code().raw());

    bool found = false;
    id_type id = 0;
    for(; it!=symbl.end(); ++it){

        if( it->value.symbol == quantity.symbol && it->owner == from) {
            id = it->id;
            found = true;
            break;
        }
    }

    eosio_assert(found, "token is not found or is not owned by account");

    // Notify both recipients
    require_recipient( from );
    require_recipient( to );

    SEND_INLINE_ACTION( *this, transferid, {from, "active"_n}, {from, to, id, memo} );
}

id_type bnft::mint( name 	owner,
                //name 	ram_payer,
                asset 	value,
                uint64_t 	uri,
                string 	tkn_name) {
    // Add token with creator paying for RAM
    id_type token_id;
    tokens.emplace( get_self(), [&]( auto& token ) {
        token_id = tokens.available_primary_key();
        token.id = token_id;
        token.uri = uri;
        token.owner = owner;
        token.value = value;
        token.tokenName = tkn_name;
        token.status = 1;
    });

    return token_id;

}

ACTION bnft::setrampayer(name payer, id_type id) {

    require_auth(payer);

    // Ensure token ID exists
    auto payer_token = tokens.find( id );
    eosio_assert( payer_token != tokens.end(), "token with specified ID does not exist" );

    // Ensure payer owns token
    eosio_assert( payer_token->owner == payer, "payer does not own token with specified ID");

    const auto& st = *payer_token;

    // Notify payer
    require_recipient( payer );

    /*tokens.erase(payer_token);
     tokens.emplace(payer, [&](auto& token){
          token.id = st.id;
          token.uri = st.uri;
          token.owner = st.owner;
          token.value = st.value;
          token.name = st.name;
     });*/

    // Set owner as a RAM payer
    tokens.modify(payer_token, payer, [&](auto& token){
        token.id = st.id;
        token.uri = st.uri;
        token.owner = st.owner;
        token.value = st.value;
        token.tokenName = st.tokenName;
        //token.verifier = st.verifier;
    });

    sub_balance( payer, st.value );
    add_balance( payer, st.value, payer );
}


ACTION bnft::burn( name verifier, id_type token_id ) {

    eosio_assert(is_valid_verifier(verifier), "invalid verifier");

    // Find token to burn
    auto burn_token = tokens.find( token_id );
    eosio_assert( burn_token != tokens.end(), "token with id does not exist" );
    //eosio_assert( burn_token->owner == verifier, "token not owned by account" );

    asset burnt_supply = burn_token->value;

    // Remove token from tokens table
    //tokens.erase( burn_token );

    auto update_token = tokens.find( token_id );
    tokens.modify( update_token, get_self(), [&]( auto& token ) {
        token.status = 0;
    });

    symbol symbolvalue = symbol("JUNGLE", 4);
    action transferInc = action(
            permission_level{get_self(),"active"_n},
            "eosio.token"_n,
            "transfer"_n,
            std::make_tuple(get_self(), burn_token->owner, asset(10000, symbolvalue), std::string("successfully transferred benefit"))
    );
    transferInc.send();


    // Lower balance from owner
    //sub_balance( burn_token->owner, burnt_supply );

    // Lower supply from currency
    sub_supply( burnt_supply );
}


void bnft::sub_balance( name owner, asset value ) {

    account_index from_acnts( _self, owner.value );
    const auto& from = from_acnts.get( value.symbol.code().raw(), "no balance object found" );
    eosio_assert( from.balance.amount >= value.amount, "overdrawn balance" );


    if( from.balance.amount == value.amount ) {
        from_acnts.erase( from );
    } else {
        from_acnts.modify( from, owner, [&]( auto& a ) {
            a.balance -= value;
        });
    }
}

void bnft::add_balance( name owner, asset value, name ram_payer ) {

    account_index to_accounts( _self, owner.value );
    auto to = to_accounts.find( value.symbol.code().raw() );
    if( to == to_accounts.end() ) {
        to_accounts.emplace( ram_payer, [&]( auto& a ){
            a.balance = value;
        });
    } else {
        to_accounts.modify( to, _self, [&]( auto& a ) {
            a.balance += value;
        });
    }
}

void bnft::sub_supply( asset quantity ) {

    auto symbol_name = quantity.symbol.code().raw();
    currency_index currency_table( _self, symbol_name );
    auto current_currency = currency_table.find( symbol_name );

    currency_table.modify( current_currency, _self, [&]( auto& currency ) {
        currency.supply -= quantity;
    });
}

void bnft::add_supply( asset quantity ) {

    auto symbol_name = quantity.symbol.code().raw();
    currency_index currency_table( _self, symbol_name );
    auto current_currency = currency_table.find( symbol_name );

    currency_table.modify( current_currency, name(0), [&]( auto& currency ) {
        currency.supply += quantity;
    });
}

ACTION bnft::notify(name user, std::string msg) {
    require_auth(get_self());
    require_recipient(user);
}

void bnft::send_summary(name user, std::string message) {
    action(
        permission_level{get_self(),"active"_n},
        get_self(),
        "notify"_n,
        std::make_tuple(user, message)
    ).send();
};

ACTION bnft::addverifier(name verifier) {
    require_auth( _self );

    auto existing_verifier = verifier_table.find( verifier.value );
    eosio_assert( existing_verifier == verifier_table.end(), "verifier already exists" );

    // Create new currency
    verifier_table.emplace( _self, [&]( auto& row ) {
        row.ver = verifier;
    });
}

bool bnft::is_valid_verifier(name verifier) {
    auto existing_verifier = verifier_table.find( verifier.value );

    if(existing_verifier == verifier_table.end()) {
        return false;
    } else {
        return true;
    }
}

EOSIO_DISPATCH( bnft, (create)(issue)(transfer)(transferid)(setrampayer)(burn)(notify)(addverifier) )
