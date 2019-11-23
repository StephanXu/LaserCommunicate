import Vue from 'vue'
import Vuex from 'vuex'
import { settingsStore } from '../rpc'

Vue.use(Vuex)
let store = new Vuex.Store({
    state: {
        isMaximized: false,
        isFocus: true,
        comRules: [],
        clientWidth: 0,
        clientHeight: 0
    },
    getters: {
        getRules: (state) => state.comRules,
        getMaximized: (state) => state.isMaximized,
        getClientWidth: (state) => state.clientWidth,
        getClientHeight: (state) => state.clientHeight,
        getFocus: (state) => state.isFocus
    },
    mutations: {
        addNewRule(state, rule) { state.comRules.push(rule) },
        setRules(state, rules) { state.comRules = rules },
        setMaximized(state, maximized) { state.isMaximized = maximized },
        setClientWidth(state, width) { state.clientWidth = width },
        setClientHeight(state, height) { state.clientHeight = height },
        setFocus(state, isFocus) { state.isFocus = isFocus }
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