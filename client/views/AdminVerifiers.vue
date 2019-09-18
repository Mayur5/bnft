<template>
  <div class="container-fluid">
    <div class="row">
      <Sidebar active="active"/>
      <div v-bind:class="[active ? 'col-9 pl-4': 'col-10 pl-4']">
        <div class="row pt-4 ">
            <div class="col">
                <h3>Admin: Verifiers</h3>
            </div>
            <div class="col">
              <button @click="openCreateVerifierModal" class="btn btn-outline-success float-right"> + Add Verifier</button>
            </div>
        </div>
        <hr>
        <div class="row" v-for="(verifier, index) in verifiers" :key="index">
          <div class="card">
            <div class="card-body">
                {{verifier.ver}}
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
import Sidebar from 'views/AdminSidebar'
import AddVerifierModal from 'views/AddVerifierModal'
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
      verifiers: []
    }
  },
  methods: {
    getVerifiers: async function(){
      const eos = window.eos;
      const result = await eos.getTableRows({
        "json": true,
        "code": "bnftcontrac1",
        "scope": "bnftcontrac1",
        "table": "verifiers",
        "limit": 100
      });
      this.verifiers = result.rows;
    },
    addVerifier: function(data, callback){
        console.log({data})
        const eos = window.eos;
        const { account } = this;
          const self = this;
          console.log(account.name)
          const transactionOptions = { authorization:[`${account.name}@${account.authority}`] };
          eos.contract("bnftcontrac1").then(async (myContract) => {
              console.log()
              try{
                const {name} = data
                console.log(name)
                let a = await myContract.addverifier({verifier: name}, transactionOptions);
                this.$notify({
                    group: 'notification',
                    title: 'Add Verifier',
                    text: 'Verifier added successfully'
                });
                // self.clear()
                self.verifiers.unshift({ver: name})
                callback()
              } catch (e){
                  console.log(e)
                  this.$notify({
                    group: 'notification',
                    title: 'Add Verifier',
                    text: 'Verifier add failed',
                    type: 'error'
                });
              }
          });
    },
    openCreateVerifierModal: function(scheme){
      const { addVerifier } = this;
      this.$modal.show(AddVerifierModal, { addVerifier }, { adaptive: true, height: "auto" })
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
              eos.contract("bnftcontrac1").then(async (myContract) => {
                  console.log({myContract})
              })
              self.getVerifiers();

          }).catch(error => {
              console.error(error);
          });
      });
    } else {
      self.getVerifiers();
    }
  }
}
</script>
