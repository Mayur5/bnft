<template>
  <div class="container-fluid">
    <div class="row">
      <Sidebar active="active"/>
      <div v-bind:class="[active ? 'col-9 pl-4': 'col-10 pl-4']">
        <div class="row pt-4 ">
          <h3>Agent: BNFTS</h3>
        </div>
        <hr>
        <div class="row mb-3" v-for="(token, index) in tokens" :key="index">
          <div class="card" style="width: 90%">
          <h5 class="card-header">{{token.tokenName}}</h5>
          <div class="card-body">
            <h5 class="card-title">Value: {{token.value}}</h5>
            <p class="card-text">Aadhaar: {{token.uri}}</p>
            <span v-bind:class="[token.status == 0 ? 'badge badge-danger': 'badge badge-primary']">{{token.status === 0 ? "Closed": "Pending Verification"}}</span>
            <!-- <a href="#" class="btn btn-outline-success float-right">Verify</a> -->
          </div>
        </div>
        </div>
      </div>
      <div class="col-3">
        <button >Toggle</button>
      </div>
    </div>
  </div>
</template>

<script>
import Sidebar from 'views/Sidebar'
import ScatterJS from 'scatterjs-core';
import ScatterEOS from 'scatterjs-plugin-eosjs';
import Eos from 'eosjs';
export default {
  data: function(){
    return {
      active: true,
      activeClass: "col-9",
      inactiveClass: "col-11",
      tokens: []
    }
  },
  methods: {
    createScheme: function(data, callback){
      let eos = window.eos;  
      const { account } = this;
      const self = this;
      const transactionOptions = { authorization:[`${account.name}@${account.authority}`] };

      eos.contract("bnftcontrac1").then(async (myContract) => {
        try {
          let result = await myContract.verify(data, transactionOptions)
          this.$notify({
            group: 'notification',
            title: 'Token Verification',
            text: 'Token verified successfully'
          });
          callback()
        } catch(e){
          console.log(e);
          this.$notify({
            group: 'notification',
            title: 'Token Verification',
            text: 'Token verification failed',
            type: 'error'
          });
        }
      });
    },
    getTokens: async function(){
      const eos = window.eos;
      const result = await eos.getTableRows({
        "json": true,
        "code": "bnftcontrac1",    // contract who owns the table
        "scope": "bnftcontrac1",   // scope of the table
        "table": "tokens",
        // "table_key":"aadhaar",
        // "index_position": "3",
        // "key_type": "i64",
        // "lower_bound": 12345672
      });
      this.tokens = result.rows;
    }
  },
  components: {
    Sidebar
  },
  mounted: function(){
    const self = this;
    if(!window.eos){
      ScatterJS.plugins(new ScatterEOS());
      const network = {
        blockchain: 'eos',
        protocol: 'http',
        host: 'dev.cryptolions.io',
        port: 38888,
        chainId: '038f4b0fc8ff18a4f0842a8f0564611f6e96e8535901dd45e43ac8691a1c4dca'
      }

      // First we need to connect to the user's Scatter.
      ScatterJS.scatter.connect('My-App').then(connected => {
          if(!connected) return false;

          const scatter = ScatterJS.scatter;
          const requiredFields = { accounts:[network] };
          scatter.getIdentity(requiredFields).then(() => {
              self.account = scatter.identity.accounts.find(x => x.blockchain === 'eos');
              const eosOptions = {};
              const eos = scatter.eos(network, Eos, eosOptions, 'http');
              window.eos = eos
              self.getTokens()
          }).catch(error => {
              console.error(error);
          });
      });
    } else {
      self.getTokens()
      console.log(window.eos)
    }
  }
}
</script>
