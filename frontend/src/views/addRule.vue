<template>
  <el-container>
    <!-- <el-header>
      <el-page-header @back="goBack" content="添加功能参数" style="margin-top:20px"></el-page-header>
    </el-header>-->
    <el-main>
      <el-form ref="form" :model="formData" label-width="100px" label-position="right">
        <el-form-item v-for="item in sheetField" :key="item.propName" :label="item.text">
          <el-input v-model="formData[item.propName]"></el-input>
        </el-form-item>

        <el-form-item>
          <el-button type="primary" @click="onSubmit">立即添加</el-button>
          <!-- <el-button @click="goBack">取消</el-button> -->
        </el-form-item>
      </el-form>
    </el-main>
  </el-container>
</template>

<script>
import global from "./constant";

export default {
  props: {
    onClose: Function
  },
  data() {
    return {
      formData: {},
      sheetField: global.sheetField
    };
  },
  methods: {
    onSubmit() {
      this.$store.dispatch("addNewRule", { ...this.formData, status: true });
      this.onClose()
      // this.$router.push({ path: "/" });
    },
    goBack() {
      // this.$router.push({ path: "/" });
    }
  }
};
</script>