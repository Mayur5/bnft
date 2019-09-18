<template>
      <div v-bind:class="[active ? 'col-2 sidebar pt-3': 'col-1 sidebar pt-3']" >
        <div class="row">
          <transition name="slide-fade">
          <div class="col-9" v-if="active">
            <ul class="list-group list-group-flush" v-if="active">
              <li class="list-group-item disabled">BNFT</li>
              <li class="list-group-item disabled">
                <router-link to="/admin/schemes">Schemes</router-link>
              </li>
              <li class="list-group-item">
                <router-link to="/admin/verifiers">Verifiers</router-link>
                
              </li>
          </ul>
          </div>
          </transition>
          <div v-bind:class="[active ? 'col-1 pt-3': 'col-12 pt-3']" style="text-align: center;">
            <span v-bind:class="[active ? 'float-right pointer': 'pointer']" href="#"  v-on:click="active = !active">&#9776;</span></div>
        </div>
      </div>
</template>
<style>
.sidebar{
  height: 100vh !important;
  border-right: 1px solid grey
}

.pointer {
  cursor: pointer;
}

.slide-fade-enter-active {
  transition: all .3s ease;
}
.slide-fade-leave-active {
  transition: all .5s ease
}
.slide-fade-enter, .slide-fade-leave-to
/* .slide-fade-leave-active below version 2.1.8 */ {
  transform: translateX(200px);
  opacity: 0;
}
</style>

<script>
export default {
  data: function(){
    return {
      active: this.$attrs.active,
      activeClass: "col-9",
      inactiveClass: "col-11"
    }
  }
}
</script>
