<template>
  <el-container>
    <el-header>
      <el-page-header @back="goBack" content="添加功能参数" style="margin-top:20px"></el-page-header>
    </el-header>
    <el-main>
      <el-form ref="form" :model="formData" label-width="100px" label-position="right">
        <el-form-item v-for="item in formData.content" :key="item.propName" :label="item.text">
          <el-input v-model="item.content"></el-input>
        </el-form-item>

        <el-form-item>
          <el-button type="primary" @click="onSubmit">立即添加</el-button>
          <el-button @click="goBack">取消</el-button>
        </el-form-item>
      </el-form>
    </el-main>
  </el-container>
</template>

<script>
import global from "./constant";

export default {
  data() {
    return {
      formData: {
        content: []
      }
    };
  },
  created() {
    for (let i = 0; i < global.sheetField.length; i++) {
      this.formData.content.push({
        propName: global.sheetField[i].propName,
        text: global.sheetField[i].text,
        content: ""
      });
    }
  },
  methods: {
    onSubmit() {
      // console.log("submit!");
      this.$router.push({
        path: "/",
        query: { newData: this.formData.content }
      });
    },
    goBack() {
      this.$router.push({
        path: "/",
        query: {}
      });
    }
  }
};
</script>