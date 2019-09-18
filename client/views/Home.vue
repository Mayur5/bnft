<template>
  <div class="container-fluid">
    <div class="row">
      <Sidebar active="active"/>
      <div v-bind:class="[active ? 'col-9 pl-4': 'col-10 pl-4']">
        <div class="row pt-4 ">
          <h3>Agent: Schemes</h3>
        </div>
        <hr>
        <div class="row" v-for="(scheme, index) in schemes" :key="index">
          <div class="card" style="width: 90%">
            <h5 class="card-header">{{scheme.schemeName}}</h5>
            <div class="card-body">
              <h5 class="card-title">Launched on {{scheme.schemeDate}}</h5>
              <h5 class="card-tex">Token: {{scheme.supply.split(" ")[1]}}</h5>
              <p class="card-text">{{scheme.schemeDesc}}</p>
              <a class="btn btn-outline-success float-right" @click="openCreateIssueModal(scheme)">Issue Token</a>
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
import IssueModal from 'views/IssueModal'
import { network } from '../constants'
import ScatterJS from 'scatterjs-core';
import ScatterEOS from 'scatterjs-plugin-eosjs';
import Eos from 'eosjs';
export default {
  data: function(){
    return {
      active: true,
      activeClass: "col-9",
      inactiveClass: "col-11",
      schemes: []
    }
  },
  methods: {
    issue: function({ to, quantity, uris, name, memo }, callback){
      let eos = window.eos;
      const { account } = this;
      const self = this;
      const transactionOptions = { authorization:[`${account.name}@${account.authority}`] };
      console.log("reached2")
      console.log(eos)
      eos.contract("bnftcontrac1").then(async (myContract) => {
        console.log(myContract)
        try{
          debugger;
          let result = await myContract.issue({ to: account.name, quantity, uris, name, memo }, transactionOptions);
          this.$notify({
            group: 'notification',
            title: 'Token Issue',
            text: 'Token issued successfully'
          });
          callback()
        } catch(e) {
          this.$notify({
            group: 'notification',
            title: 'Token Issue',
            text: 'Token issue failed',
            type: 'error'
          });
        }
      });
    },
    getSchemes: async function(){
      const eos = window.eos;
      const result = await eos.getTableRows({
        "json": true,
        "code": "bnftcontrac1",
        "scope": "BNFTAA",
        "table": "statnew"
      });
      this.schemes = result.rows;
    },
    openCreateIssueModal: function(scheme){
      const { issue } = this;
      this.$modal.show(IssueModal, { issue, asset: scheme.supply.split(" ")[1]}, { adaptive: true, height: "auto" })
    }
  },
  components: {
    Sidebar
  },
  mounted: function(){
    const self = this;
    if(!window.eos){
      ScatterJS.plugins(new ScatterEOS());
      ScatterJS.scatter.connect('My-App').then(connected => {
          if(!connected) return false;
          const scatter = ScatterJS.scatter;
          const requiredFields = { accounts:[network] };
          scatter.getIdentity(requiredFields).then(() => {
              const account = scatter.identity.accounts.find(x => x.blockchain === 'eos');
              self.account = account;
              const eosOptions = { };
              const eos = scatter.eos(network, Eos, eosOptions, 'http');
              window.eos = eos;
              self.getSchemes();

          }).catch(error => {
              console.error(error);
          });
      });
    } else {
      self.getSchemes();
    }
  }
}
</script>
