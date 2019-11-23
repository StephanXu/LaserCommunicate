<template>
  <div id="title-bar">
    <a class="title-text">仪表盘</a>

    <a class="control_btn shutdown_btn text-center" @click="controlBtn('shutdown');">
      <i class="codicon codicon-chrome-close" style="line-height:35px"></i>
    </a>
    <a class="control_btn maximize_btn text-center" @click="controlBtn('max');">
      <i v-if="getMaximized" class="codicon codicon-chrome-restore" style="line-height:35px"></i>
      <i v-else class="codicon codicon-chrome-maximize" style="line-height:35px"></i>
    </a>
    <a class="control_btn minimize_btn text-center" @click="controlBtn('mini')">
      <i class="codicon codicon-chrome-minimize" style="line-height:35px"></i>
    </a>
  </div>
</template>

<script>
import Vue from "vue";
import { clientCommand } from "../rpc";
import {mapGetters} from 'vuex'
export default Vue.extend({
  component: "title-bar",
  computed:{
    ...mapGetters([
      'getMaximized'
    ])
  },
  methods: {
    controlBtn(buttonType) {
      clientCommand.systemButton(buttonType);
    }
  }
});
</script>

<style scoped>
#title-bar {
  -webkit-app-region: drag;
  height: 35px;
  background-color: #2196f3;
  box-shadow: 0 1px 3px rgba(0, 0, 0, 0.2), 0 1px 1px rgba(0, 0, 0, 0.14),
    0 2px 1px -1px rgba(0, 0, 0, 0.12);
}

.title-text {
  line-height: 35px;
  padding-left: 0.7em;
  color: #fff;
  font-size: 0.8em;
  font-family: "Helvetica Neue", Helvetica, "PingFang SC", "Hiragino Sans GB",
    "Microsoft YaHei", "微软雅黑", Arial, sans-serif;
}

.control-btn-icon{
  line-height: 35px;
}

.control_btn {
  color: rgb(255, 255, 255);
  position: relative;
  float: right;
  top: 0px;
  width: 45px;
  height: 35px;
  line-height: 35px;
  font-size: 1.1em;
  z-index: 999;
  transition: 0.5s;
  -webkit-app-region: no-drag;
}

.control_btn:hover {
  color: rgb(255, 255, 255);
  position: relative;
  float: right;
  right: 0px;
  top: 0px;
  width: 45px;
  height: 35px;
  line-height: 35px;
  z-index: 999;
  font-size: 1.1em;
  transition: 0.5s;
  -webkit-app-region: no-drag;
}

.shutdown_btn {
  background-color: #2196f3;
  right: 0px;
}

.shutdown_btn:hover {
  background: #cd1a2b;
  right: 0px;
}

.maximize_btn {
  background-color: #2196f3;
  right: 0px;
  font-size: 0.9em;
}

.maximize_btn:hover {
  background: #0d47a1;
  right: 0px;
  font-size: 0.9em;
}

.minimize_btn {
  background-color: #2196f3;
  right: 0px;
  font-size: 0.9em;
}

.minimize_btn:hover {
  background: #0d47a1;
  right: 0px;
  font-size: 0.9em;
}

.text-center {
  text-align: center;
}
</style>