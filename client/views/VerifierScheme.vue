<template>
  <div class="container-fluid">
    <div class="row">
      <Sidebar active="active"/>
      <div v-bind:class="[active ? 'col-9 pl-4': 'col-10 pl-4']">
        <div class="row pt-4 ">
            <div class="col">
                <h3>Verifier</h3>
            </div>
            
        </div>
        <hr>
        <div class="row mb-3" v-for="(token, index) in schemes" :key="index">
          <div class="card" style="width: 90%">
          <h5 class="card-header">{{token.tokenName}}</h5>
          <div class="card-body">
            <h5 class="card-title">Value: {{token.value}}</h5>
            <p class="card-text">Aadhaar: {{token.uri}}</p>
            <span v-bind:class="[token.status == 0 ? 'badge badge-danger': 'badge badge-primary']">{{token.status === 0 ? "Closed": "Pending Verification"}}</span>
            <a v-if="token.status == 1" class="btn btn-outline-success float-right" @click="verify(token)">Verify</a>
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
import Sidebar from 'views/SidebarV'
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
      schemes: [],
      account: null
    }
  },
  methods: {
    getSchemes: async function(){
      const eos = window.eos;
      const result = await eos.getTableRows({
        "json": true,
        "code": "bnftcontrac1",
        "scope": "bnftcontrac1",
        "table": "tokens",
        "limit": 100
      });
      this.schemes = result.rows;
    },
    openCreateIssueModal: function(scheme){
      const { issue } = this;
      this.$modal.show(IssueModal, { issue, asset: scheme.supply.split(" ")[1]}, { adaptive: true, height: "auto" })
    },
    verify: function(token){
        const eos = window.eos;
        const { account } = this;
          const self = this;
          const transactionOptions = { authorization:[`${account.name}@${account.authority}`] };
          eos.contract("bnftcontrac1").then(async (myContract) => {
              try {
                  let result = await myContract.burn({"token_id": token.id, "owner": account.name}, transactionOptions);
                  this.$notify({
                    group: 'notification',
                    title: 'Verify BNFT',
                    text: 'BNFT verified and burnt'
                });
              } catch(e){
                  this.$notify({
                    group: 'notification',
                    title: 'Verify BNFT',
                    text: 'BNFT verification failed',
                    type: "error"
                });
              }
          });
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
