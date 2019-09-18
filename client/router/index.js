import Vue from 'vue'
import Router from 'vue-router'
import Home from '../views/Home'
import HomePage from '../views/HomePage'
import Token from '../views/Token'
import Scheme from '../views/Scheme'
import AdminScheme from '../views/AdminScheme'
import VerifierScheme from '../views/VerifierScheme'
import AdminVerifiers from '../views/AdminVerifiers'

Vue.use(Router)

export default new Router({
    mode: 'hash',
    routes: [{
            path: '/',
            component: HomePage
        },
        {
            path: '/admin/add_scheme',
            component: Scheme
        },
        {
            path: '/admin/schemes',
            component: AdminScheme
        },
        {
            path: '/admin/verifiers',
            component: AdminVerifiers
        },
        {
            path: '/agent/tokens',
            component: Token
        },
        {
            path: '/agent/schemes',
            component: Home
        },
        {
            path: '/verifier/tokens',
            component: VerifierScheme
        }
    ]
})