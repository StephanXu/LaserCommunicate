import Vue from 'vue'
import VueRouter from 'vue-router'
import Addevent from '../views/addevent.vue'
import Table from '../views/table.vue'
import Temperature from '../views/temperature.vue'

Vue.use(VueRouter)

const routerPush = VueRouter.prototype.push
VueRouter.prototype.push = function push(location) {
  return routerPush.call(this, location).catch(error=> error)
}
const routes = [
  {
    path: '/',
    name: 'table',
    component: Table
  },
  {
    path: '/addevent',
    name: 'addevent',
    component: Addevent
  },
  {
    path: '/temperature',
    name: 'temperature',
    component: Temperature
  }
  
]

const router = new VueRouter({
  routes
})

export default router
