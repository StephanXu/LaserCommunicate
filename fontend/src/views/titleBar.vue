<template>
  <div id="title-bar">
    <div class="app-icon">
      <i class="codicon codicon-list-flat" style="line-height:35px"></i>
    </div>
    <a class="title-text">仪表盘</a>
    <div class="control-btn-container" :style="controlButtonContainerWidth">
      <a class="control_btn minimize_btn text-center" @click="controlBtn('mini')">
        <i class="codicon codicon-chrome-minimize" style="line-height:35px"></i>
      </a>
      <a class="control_btn maximize_btn text-center" @click="controlBtn('max');">
        <i :class="maximizeButtonClass" style="line-height:35px"></i>
      </a>
      <a class="control_btn shutdown_btn text-center" @click="controlBtn('shutdown');">
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
    ...mapGetters(["getMaximized", "getClientWidth"]),
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
#title-bar {
  -webkit-app-region: drag;
  height: 35px;
  background-color: #2b579a;
  display: flex;
  display: -webkit-flex;
}

.title-text {
  line-height: 35px;
  color: #fff;
  font-size: 0.8em;
  font-family: "Helvetica Neue", Helvetica, "PingFang SC", "Hiragino Sans GB",
    "Microsoft YaHei", "微软雅黑", Arial, sans-serif;
  width: 100px;
}

.app-icon {
  line-height: 35px;
  color: #fff;
  width: 35px;
  text-align: center;
}

.control-btn-container {
  display: flex;
  flex-wrap: nowrap;
  justify-content: flex-end;
}

.control_btn {
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
}

.control_btn:hover {
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
}

.shutdown_btn {
  background-color: #2b579a;
  /* right: 0px; */
}

.shutdown_btn:hover {
  background: #e81123;
  /* right: 0px; */
}

.maximize_btn {
  background-color: #2b579a;
  /* right: 0px; */
  font-size: 0.9em;
}

.maximize_btn:hover {
  background: #124078;
  /* right: 0px; */
  font-size: 0.9em;
}

.minimize_btn {
  background-color: #2b579a;
  /* right: 0px; */
  font-size: 0.9em;
}

.minimize_btn:hover {
  background: #124078;
  /* right: 0px; */
  font-size: 0.9em;
}

.text-center {
  text-align: center;
}
</style>