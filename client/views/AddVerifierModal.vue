<template>
    <div class="row pt-3 pb-3 pl-3 pr-3">
        <div class="col">
            <form>
                <div class="form-group">
                    <label for="">Name</label>
                    <input type="text" class="form-control" v-model="name">
                </div>
                <button class="btn btn-outline-primary" @click="submitData">Add verifier</button>
            </form>
        </div>
    </div>
</template>


<style>
</style>


<script>

export default {
  name: 'AddVerifierModal',
  data () {
    return {
        name: ""
    }
  },
  methods: {
      submitData: function(){
          const addVerifier = this.$attrs.addVerifier;
          const { name } = this;
          const self = this;
          addVerifier({ name }, function(){
              self.$emit('close');
          });
      }
  }
}
</script>
