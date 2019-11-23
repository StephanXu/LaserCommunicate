<template>
  <div id="app">
    <title-bar />
    <navigate-bar />
    <el-container>
      <el-scrollbar :style="mainViewStyle">
        <router-view></router-view>
      </el-scrollbar>
    </el-container>
  </div>
</template>

<script>
import TitleBar from "./components/titleBar";
import NavigateBar from "./components/navigateBar";
import { mapGetters } from "vuex";
export default {
  components: {
    "title-bar": TitleBar,
    "navigate-bar": NavigateBar
  },
  data() {
    return {};
  },
  computed: {
    ...mapGetters(["getMaximized", "getClientWidth", "getClientHeight"]),
    mainViewStyle() {
      return {
        height: `${this.getClientHeight - 35 - 40}px`,
        width: `${this.getClientWidth}px`
      };
    }
  },
  mounted() {
    this.$store.commit(
      "setClientHeight",
      document.documentElement.clientHeight
    );
    this.$store.commit("setClientWidth", document.documentElement.clientWidth);
    const _this = this;
    window.onresize = function temp() {
      _this.$store.commit(
        "setClientHeight",
        document.documentElement.clientHeight
      );
      _this.$store.commit(
        "setClientWidth",
        document.documentElement.clientWidth
      );
    };
  }
};
</script>

<style>
body {
  margin: 0px;
  overflow-y: hidden;
  overflow-x: hidden;
  user-select: none;
}
</style>