<template>
  <el-menu
    :default-active="activeIndex"
    mode="horizontal"
    @select="handleSelect"
    :background-color="menuBackgroundColor"
    :text-color="menuItemTextColor"
    :active-text-color="menuItemTextActiveColor"
  >
    <el-menu-item index="temperature">仪表盘</el-menu-item>
    <el-menu-item index="table">规则选项</el-menu-item>
    <el-menu-item index="about">关于</el-menu-item>
  </el-menu>
</template>

<script>
import { mapGetters } from "vuex";

export default {
  computed: {
    ...mapGetters(["getFocus"]),
    menuBackgroundColor() {
      return this.getFocus ? "#2b579a" : "#dddddd";
    },
    menuItemTextColor() {
      return this.getFocus ? "#ddd" : "#7b7b7b";
    },
    menuItemTextActiveColor(){
      return this.getFocus ? "#fff" : "#111111";
    }
  },
  data() {
    return {
      activeIndex: "table"
    };
  },
  methods: {
    handleSelect(key) {
      this.$router.push({ name: key });
    }
  }
};
</script>

<style scoped>
</style>