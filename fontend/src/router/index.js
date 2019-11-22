import Vue from 'vue'
import VueRouter from 'vue-router'
import AddEvent from '../views/addEvent'
import Table from '../views/table'
import Temperature from '../views/temperature'

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
    path: '/addEvent',
    name: 'addEvent',
    component: AddEvent
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
