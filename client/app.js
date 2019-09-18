import Vue from 'vue'
import { sync } from 'vuex-router-sync'
import App from './components/App'
import Notifications from 'vue-notification'

import router from './router'
import store from './store'
import VModal from 'vue-js-modal'

sync(store, router)

Vue.use(VModal, { dynamic: true, dialog: true });
Vue.use(Notifications);



const app = new Vue({
    router,
    store,
    ...App
})

export { app, router, store }