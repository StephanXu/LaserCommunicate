import Vue from 'vue'
import Vuex from 'vuex'
import { settingsStore } from '../rpc'

Vue.use(Vuex)
let store = new Vuex.Store({
    state: {
        isMaximized: false,
        comRules: [],
        clientWidth: 0,
        clientHeight: 0
    },
    getters: {
        getRules(state) { return state.comRules },
        getMaximized(state) { return state.isMaximized },
        getClientWidth(state) { return state.clientWidth },
        getClientHeight(state) { return state.clientHeight }
    },
    mutations: {
        addNewRule(state, rule) { state.comRules.push(rule) },
        setRules(state, rules) { state.comRules = rules },
        setMaximized(state, maximized) { state.isMaximized = maximized },
        setClientWidth(state, width) { state.clientWidth = width; },
        setClientHeight(state, height) { state.clientHeight = height; }
    },
    actions: {
        addNewRule(context, rule) {
            context.commit('addNewRule', rule)
            settingsStore.setRules(context.state.comRules)
        },
        loadRules(context) {
            context.commit('setRules', settingsStore.loadRules())
        }
    }
})

export default store