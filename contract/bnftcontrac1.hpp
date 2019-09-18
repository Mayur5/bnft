#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <string>
#include <vector>

using namespace eosio;
using std::string;
using std::vector;
typedef uint128_t uuid;
typedef uint64_t id_type;
typedef string uri_type;

CONTRACT bnft : public eosio::contract {

public:
using contract::contract;
bnft( name receiver, name code, datastream<const char*> ds)
: contract(receiver, code, ds), tokens(receiver, receiver.value), verifier_table(receiver, receiver.value) {}


ACTION create(name issuer, std::string symbol, std::string scheme_name, std::string scheme_desc, std::string scheme_date );

ACTION issue(name to,
asset quantity,
        vector<uint64_t> uris,
string name,
        string memo);

ACTION transferid(name from,
name to,
        id_type id,
string memo);

ACTION transfer(name from,
name to,
        asset quantity,
string memo);

ACTION burn(name owner,
id_type token_id);

ACTION setrampayer(name payer, id_type id);

ACTION notify(name user, string msg);

ACTION addverifier(name verifier);


TABLE account {

        asset balance;

        uint64_t primary_key() const { return balance.symbol.code().raw(); }
};


TABLE stats {
        asset supply;
        name issuer;
        string schemeName;
        string schemeDesc;
        string schemeDate;

        uint64_t primary_key() const { return supply.symbol.code().raw(); }
        uint64_t get_issuer() const { return issuer.value; }
};


TABLE token {
        id_type id;          // Unique 64 bit identifier,
        /*
         * ,
        {
            "new_type_name": "uri_type",
            "type": "string"
        }
         */
        uint64_t uri;        // RFC 3986
        name owner;  	 // token owner
        asset value;         // token value (1 SYS)
        string tokenName;	 // token name
        uint64_t status;

        id_type primary_key() const { return id; }
        uint64_t get_owner() const { return owner.value; }
        uint64_t get_uri() const { return uri; }
        asset get_value() const { return value; }
        uint64_t get_symbol() const { return value.symbol.code().raw(); }
        string get_name() const { return tokenName; }
        //uint64_t get_verifier() const { return verifier.value; }

        // generated token global uuid based on token id and
        // contract name, passed in the argument
        uuid get_global_id(name self) const
        {
            uint128_t self_128 = static_cast<uint128_t>(self.value);
            uint128_t id_128 = static_cast<uint128_t>(id);
            uint128_t res = (self_128 << 64) | (id_128);
            return res;
        }

        string get_unique_name() const
        {
            string unique_name = tokenName + "#" + std::to_string(id);
            return unique_name;
        }
};

TABLE mapping {
        //token id
        id_type id;
        asset sym;
        uint64_t aadhaar;
        uint64_t status;

        id_type primary_key() const { return id; }
        uint64_t get_symbol() const { return sym.symbol.code().raw(); }
        uint64_t get_aadhaar() const { return aadhaar; }
};

TABLE verifier {
       name ver;

       uint64_t primary_key() const { return ver.value; }
};

using account_index = eosio::multi_index<"accounts"_n, account>;

using currency_index = eosio::multi_index<"statnew"_n, stats,
        indexed_by< "byissuer"_n, const_mem_fun< stats, uint64_t, &stats::get_issuer> > >;

using token_index = eosio::multi_index<"tokens"_n, token,
        indexed_by< "byowner"_n, const_mem_fun< token, uint64_t, &token::get_owner> >,
        indexed_by< "bysymbol"_n, const_mem_fun< token, uint64_t, &token::get_symbol> > >;

using mapping_index = eosio::multi_index<"mappingnew"_n, mapping,
        indexed_by< "bysymbol"_n, const_mem_fun< mapping, uint64_t, &mapping::get_symbol > >,
        indexed_by< "byaadhaar"_n, const_mem_fun< mapping, uint64_t, &mapping::get_aadhaar > > >;

using verifier_index = eosio::multi_index<"verifiers"_n, verifier>;

private:
token_index tokens;
verifier_index verifier_table;

id_type mint(name owner, /*name ram_payer,*/ asset value, uint64_t uri, string name);

void sub_balance(name owner, asset value);
void add_balance(name owner, asset value, name ram_payer);
void sub_supply(asset quantity);
void add_supply(asset quantity);
void send_summary(name user, string msg);
bool is_valid_verifier(name verifier);
};