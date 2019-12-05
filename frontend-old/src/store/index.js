import Vue from 'vue'
import Vuex from 'vuex'
import { Message } from 'element-ui'
import {
    settingsStore
} from '../rpc'
import { get, post } from "../http/http"

Vue.use(Vuex)
let store = new Vuex.Store({
    state: {
        connectStatus: false,
        connectPort: '',
        isMaximized: false,
        isFocus: true,
        tableData: [],
        comRules: [],
        clientWidth: 0,
        clientHeight: 0
    },
    getters: {
        getTable: (state) => state.tableData,
        getConnectPort: (state) => state.connectPort,
        getConnectStatus: (state) => state.connectStatus,
        getRules: (state) => state.comRules,
        getMaximized: (state) => state.isMaximized,
        getClientWidth: (state) => state.clientWidth,
        getClientHeight: (state) => state.clientHeight,
        getFocus: (state) => state.isFocus,
        getSpecifyValue: (state) => {
            return (symbol) => {
                return state.tableData.find((item) => (item.symbol === symbol))
            }
        }
    },
    mutations: {
        getTableData(state, rule) {
            state.tableData = rule
        },
        connectPort(state, rule) {
            state.connectPort = rule
        },
        connectStatus(state, rule) {
            state.connectStatus = rule
        },
        addNewRule(state, rule) {
            state.comRules.push(rule)
        },
        removeRule(state, rule) {
            state.comRules.splice(state.comRules.findIndex((val) => val === rule), 1)
        },
        setRules(state, rules) {
            state.comRules = rules
        },
        setMaximized(state, maximized) {
            state.isMaximized = maximized
        },
        setClientWidth(state, width) {
            state.clientWidth = width
        },
        setClientHeight(state, height) {
            state.clientHeight = height
        },
        setFocus(state, isFocus) {
            state.isFocus = isFocus
        }
    },
    actions: {
        // 获取温度数据
        refreshSpecifyData(context, symbol) {
            let index = context.state.tableData.findIndex((item) => (item.symbol === symbol))
            get('/api/interface/' + context.state.tableData[index].id).then(res => {
                context.state.tableData[index]['data'] = res.content
            })
        },
        // 获取功能参数列表表格数据
        getTableData(context, rule) {
            get("/api/interface", rule
                , { headers: { "Content-Type": "application/json" } }
            ).then(response => {
                var res = response.action
                // get('/api/interface/all').then(response => {
                //     console.log(response)
                // })
                for (let i = 0; i < 10; i++) {
                    Vue.set(res[i], 'index', i + 1)
                    get('/api/interface/' + res[i].id).then(response => {
                        Vue.set(res[i], 'data', response.content)
                    })
                }
                context.commit('getTableData', res)
            }).catch(() => {
                Message.warning("获取数据失败,请先选择串口连接！");
            })
            settingsStore.setRules(context.state.tableData)
        },
        // 初始化连接
        initConnect(context, value) {
            context.commit('connectPort', value)
            var body = {
                "action": "activate",
                "deviceName": value,
                "baudrate": "115200",
                "parity": "78",
                "dataBit": "8",
                "stopBit": "0",
                "slaveId": "11"
            }
            post("/api/com/", JSON.stringify(body)
                , { headers: { "Content-Type": "application/json" } }
            ).then(() => {
                context.commit('connectStatus', true)
                Message.success("设备连接成功！")
            }).catch(() => {
                Message.warning("连接失败，请选择正确的串口！")
            })
        },
        // 断开连接
        disConnect(context, value) {
            var body = {
                action: "deactivate",
                deviceName: value,
                baudrate: "115200",
                parity: "78",
                dataBit: "8",
                stopBit: "0",
                slaveId: "11"
            };
            post("/api/com/", JSON.stringify(body), {
                headers: { "Content-Type": "application/json" }
            })
                .then(() => {
                    context.commit('connectStatus', false)
                    Message.success("断开连接成功！");
                })
                .catch(() => {
                    Message.error("断开连接失败！");
                })
        },
        addNewRule(context, rule) {
            context.commit('addNewRule', rule)
            settingsStore.setRules(context.state.comRules)
        },
        loadRules(context) {
            context.commit('setRules', settingsStore.loadRules())
        },
        removeRule(context, rule) {
            context.commit('removeRule', rule)
            settingsStore.setRules(context.state.comRules)
        }
    }
})

export default store