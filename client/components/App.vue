<template>
  <div id="app">
    <router-view></router-view>
    <modals-container/>
    <notifications group="notification" />

  </div>
</template>


<style>

body {
  margin: 0;
  font-size: 2rem;
  font-family: -apple-system, BlinkMacSystemFont,
               'avenir next', avenir,
               helvetica, 'helvetica neue',
               Ubuntu,
               'segoe ui', arial,
               sans-serif;
}
.page {
  text-align: center;
  /* nesting for the need to test postcss */
}
</style>
