<template>
  <div id="app">
    <title-bar />
    <router-view></router-view>
  </div>
</template>

<script>
import titleBar from "./views/titleBar";
export default {
  components: {
    "title-bar": titleBar
  }
};
</script>

<style>
    body{
      margin:0px;
    }
</style>