import Vue from 'vue'
import Vuex from 'vuex'
import { settingsStore } from '../rpc'

Vue.use(Vuex)
let store = new Vuex.Store({
    state: {
        isMaximized: false,
        comRules: []
    },
    getters: {
        getRules(state) {
            return state.comRules
        },
        getMaximized(state) {
            return state.isMaximized
        }
    },
    mutations: {
        addNewRule(state, rule) {
            state.comRules.push(rule)
        },
        setRules(state, rules) {
            state.comRules = rules
        },
        setMaximized(state, maximized) {
            state.isMaximized = maximized
        }
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