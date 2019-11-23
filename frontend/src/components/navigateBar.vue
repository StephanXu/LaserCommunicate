<template>
  <el-menu :default-active="activeIndex" mode="horizontal" @select="handleSelect">
    <el-menu-item index="temperature">仪表盘</el-menu-item>
    <el-menu-item index="table">规则选项</el-menu-item>
    <el-menu-item index="about">关于</el-menu-item>
  </el-menu>
</template>

<script>
export default {
  data() {
    return {
      activeIndex: "2"
    };
  },
  methods: {
    handleSelect(key) {
      this.$router.push({ name: key });
    }
  }
};
</script>