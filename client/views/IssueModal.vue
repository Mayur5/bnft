<template>
<div class="row pt-3 pb-3 pl-3 pr-3">
    <div class="col">
        <h3>Issue token</h3>
        <hr>
        <form>
            <div class="form-group">
                <label for="">Aadhaar</label>
                <input type="text" class="form-control" v-model="uris">
            </div>
            <button class="btn btn-outline-primary" @click="issue">Issue</button>
        </form>
    </div>
</div>
</template>


<style>
</style>


<script>

export default {
  name: 'IssueModal',
  data () {
    return {
        name: "",
        to: "",
        asset: this.$attrs.asset,
        quantity: "",
        uris: "",
        memo: ""
    }
  },
  methods: {
      issue: function(){
          console.log("reached")
          const self = this;
          const issueScheme = this.$attrs.issue;
          const { name, asset, quantity, to, memo, uris } = this;
          console.log({name: "name", uris: uris.split(","), quantity: `${uris.split(",").length} ${asset}`, memo: "memeo", to })
          console.log(issueScheme)
          issueScheme({name: "name", uris: uris.split(","), quantity: `${uris.split(",").length} ${asset}`, memo: "memeo", to }, function(){
              self.$emit('close')
          });
      }
  }
}
</script>
