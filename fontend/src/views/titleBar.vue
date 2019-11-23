<template>
  <div :class="barClass">
    <div :class="{'app-icon':true,'window-lost-focus':!getFocus}">
      <i class="codicon codicon-list-flat" style="line-height:35px"></i>
    </div>
    <a :class="{'title-text':true,'window-lost-focus':!getFocus}">仪表盘</a>
    <div class="control-btn-container" :style="controlButtonContainerWidth">
      <a :class="controlButtonClass" @click="controlBtn('mini')">
        <i class="codicon codicon-chrome-minimize" style="line-height:35px"></i>
      </a>
      <a :class="controlButtonClass" @click="controlBtn('max');">
        <i :class="maximizeButtonClass" style="line-height:35px"></i>
      </a>
      <a :class="{...controlButtonClass,'control-btn-close':true}" @click="controlBtn('shutdown');">
        <i class="codicon codicon-chrome-close" style="line-height:35px"></i>
      </a>
    </div>
  </div>
</template>

<script>
import Vue from "vue";
import { clientCommand } from "../rpc";
import { mapGetters } from "vuex";

export default Vue.extend({
  component: "title-bar",
  data() {
    return {
      clientWidth: 0
    };
  },
  computed: {
    ...mapGetters(["getMaximized", "getClientWidth", "getFocus"]),
    controlButtonContainerWidth() {
      return {
        width: `${this.getClientWidth - 100 - 35}px`
      };
    },
    maximizeButtonClass() {
      return {
        codicon: true,
        "codicon-chrome-restore": this.getMaximized,
        "codicon-chrome-maximize": !this.getMaximized
      };
    },
    controlButtonClass() {
      return {
        "control-btn": true,
        "window-lost-focus": !this.getFocus
      };
    },
    barClass() {
      return {
        "title-bar": true,
        "title-bar-window-lost-focus": !this.getFocus
      };
    }
  },
  methods: {
    controlBtn(buttonType) {
      clientCommand.systemButton(buttonType);
    }
  }
});
</script>

<style scoped>
.title-bar {
  -webkit-app-region: drag;
  height: 35px;
  background-color: #2b579a;
  display: flex;
  display: -webkit-flex;
  color: #000;
  transition: 0.3s;
}

.title-text {
  line-height: 35px;
  color: #fff;
  font-size: 0.8em;
  font-family: "Helvetica Neue", Helvetica, "PingFang SC", "Hiragino Sans GB",
    "Microsoft YaHei", "微软雅黑", Arial, sans-serif;
  width: 100px;
  transition: 0.3s;
}

.app-icon {
  line-height: 35px;
  color: #fff;
  width: 35px;
  text-align: center;
  transition: 0.3s;
}

.control-btn-container {
  display: flex;
  flex-wrap: nowrap;
  justify-content: flex-end;
}

.control-btn {
  color: rgb(255, 255, 255);
  position: relative;
  top: 0px;
  width: 45px;
  height: 35px;
  line-height: 35px;
  font-size: 1.1em;
  z-index: 999;
  transition: 0.5s;
  -webkit-app-region: no-drag;
  display: inline-block;
  /* background-color: #2b579a; */
  text-align: center;
}

.control-btn:hover {
  color: rgb(255, 255, 255);
  position: relative;
  right: 0px;
  top: 0px;
  width: 45px;
  height: 35px;
  line-height: 35px;
  z-index: 999;
  font-size: 1.1em;
  transition: 0.5s;
  -webkit-app-region: no-drag;
  display: inline-block;
  background-color: #124078;
  font-size: 0.9em;
  text-align: center;
}

.window-lost-focus {
  color: #7b7b7b;
  transition: 0.3s;
}

.window-lost-focus:hover {
  background-color: #c6c6c6;
  transition: 0.3s;
}

.control-btn-close:hover {
  background-color: #e81123;
}

.title-bar-window-lost-focus{
  background-color: #dddddd;
  transition: 0.3s;
}

</style>