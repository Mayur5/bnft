<template>
  <div class="container-fluid">
      <div class="row">
          <div class="col-5"></div>
          <div class="col-4">
            <h2>I am </h2>
            <div class="row pb-3">
                <router-link to="/agent/schemes" class="btn btn-outline-primary btn-lg">Agent</router-link> &nbsp;&nbsp;&nbsp;
                <router-link to="/verifier/tokens" class="btn btn-outline-primary btn-lg">Verifier</router-link>&nbsp;&nbsp;&nbsp;
                <router-link to="/admin/schemes" class="btn btn-outline-primary btn-lg">Admin</router-link>
            </div>
            <div class="row pb-3">
            </div>
            <div class="row pb-3">
            </div>
          </div>
      </div>
  </div>
</template>

<script>
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
  },
  components: {

  },
  mounted: function(){
  }
}
</script>
