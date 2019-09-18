<template>
  <div class="container-fluid">
      <div class="row pt-3">
          <div class="col-3"></div>
          <div class="col-6">
              <h3>Create Scheme</h3>
              <form>
                  <div class="form-group">
                      <label for="">Name</label>
                      <input type="text" class="form-control" v-model="schemeName">
                  </div>
                  <div class="form-group">
                      <label for="">Description</label>
                      <input type="text" class="form-control" v-model="schemeDesc">
                  </div>
                  <div class="form-group">
                      <label for="">Symbol</label>
                      <input type="text" class="form-control" v-model="symbol">
                  </div>
                  <div class="form-group">
                      <label for="">Date</label>
                      <input type="text" class="form-control" v-model="schemeDate">
                  </div>
                  <button class="btn btn-outline-primary" v-on:click="createScheme">
                      Create Scheme
                  </button>
                  <button class="btn btn-outline-danger" @click="$router.go(-1)">Back</button>
              </form>
          </div>
      </div>
  </div>
</template>

<script>
import Sidebar from 'views/Sidebar'
import { network } from '../constants'
import ScatterJS from 'scatterjs-core';
import ScatterEOS from 'scatterjs-plugin-eosjs';
import Eos from 'eosjs';
export default {
  data: function(){
    return {
        schemeName: "",
        schemeDesc: "",
        schemeDate: "",
        symbol: "",
        account: null
    }
  },
  methods: {
      createScheme: function(){
          const eos = window.eos;
          const { schemeName, schemeDesc, schemeDate, symbol } = this;
          const { account } = this;
          const self = this;
          console.log(account.name)
          const transactionOptions = { authorization:[`${account.name}@${account.authority}`] };

          eos.contract("bnftcontrac1").then(async (myContract) => {
              try{
                let a = await myContract.create({ issuer: account.name, scheme_name: schemeName, scheme_desc: schemeDesc, scheme_date: schemeDate, symbol }, transactionOptions)
                this.$notify({
                    group: 'notification',
                    title: 'Scheme Creation',
                    text: 'Scheme created successfully'
                });
                self.clear()
              } catch (e){
                  this.$notify({
                    group: 'notification',
                    title: 'User Creation',
                    text: 'Scheme creation failed',
                    type: 'error'
                });
              }
          });
      },
      clear: function(){
        this.schemeName = "";
        this.schemeDesc = "";
        this.schemeDate = "";
        this.symbol = "";
      }
  },
  components: {

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
          scatter.getIdentity(requiredFields).then((id) => {
              console.log(id)
              console.log(scatter.identity.accounts)
              const account = scatter.identity.accounts.find(x => x.blockchain === 'eos');
              self.account = account;
              console.log({account})
              console.log(account.name)
              const eosOptions = { };
              const eos = scatter.eos(network, Eos, eosOptions, 'http');
              window.eos = eos
              console.log(window.eos)
            //   self.getTokens()
          }).catch(error => {
              console.error(error);
          });
      });
    } else {
    //   self.getTokens()
      console.log(window.eos)
    }
  }
}
</script>
