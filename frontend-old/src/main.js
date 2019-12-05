import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import ElementUI from 'element-ui'
import echarts from 'echarts'
import 'element-ui/lib/theme-chalk/index.css'
import './icon/codicon.css'
import { mapGetters } from "vuex";


Vue.prototype.$echarts = echarts
Vue.use(ElementUI)
Vue.config.productionTip = false
new Vue({
  el: '#app',
  router,
  store,
  render: h => h(App),
  computed: {
    ...mapGetters(["getConnectPort"])
  },
  created() {
    this.$store.dispatch('loadRules')
    this.$store.dispatch("getTableData", {mode:0});
  },
  destroyed(){
    this.$store.dispatch('disConnect',this.getConnectPort)
  },
})

