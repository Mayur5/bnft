import './promise-polyfill'
import { app } from './app'

// Enable progressive web app support (with offline-plugin)
if (process.env.NODE_ENV === 'production') {
  require('./pwa')
}

require('../node_modules/bootstrap/dist/css/bootstrap.css')
app.$mount('#app')
