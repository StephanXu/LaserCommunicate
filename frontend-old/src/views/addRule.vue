<template>
  <el-container>
    <!-- <el-header>
      <el-page-header @back="goBack" content="添加功能参数" style="margin-top:20px"></el-page-header>
    </el-header>-->
    <el-main>
      <el-form
        ref="formData"
        :rules="rules"
        :model="formData"
        label-width="100px"
        label-position="right"
      >
        <!-- <el-form-item v-for="item in sheetField" :key="item.propName" :label="item.text">
          <el-input v-model="formData[item.propName]"></el-input>
        </el-form-item>-->
        <el-form-item label="数据名称:">
          <span>{{formData.symbol}}</span>
        </el-form-item>
        <el-form-item label="数据描述:">
          <span>{{formData.desc}}</span>
        </el-form-item>
        <el-form-item label="当前数值:">
          <span>{{formData.data}}</span>
        </el-form-item>
        <el-form-item label="设置数值" prop="dataValue">
          <el-input v-model="formData.dataValue"></el-input>
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="onSubmit('formData')" icon="el-icon-check">立即发送</el-button>
          <el-button type="danger" @click="resetForm('formData')" icon="el-icon-close">取消</el-button>
        </el-form-item>
      </el-form>
    </el-main>
  </el-container>
</template>

<script>
import global from "./constant";

export default {
  props: {
    onClose: Function,
    row: Object
  },
  data() {
    //判断字符串是否为数字 （判断正整数 /^[1-9]+[0-9]*]*$/）
    var checkNum = (rule, num, callback) => {
      var re = /^[0-9]+.?[0-9]*$/;
      if (!re.test(num)) {
        callback(new Error("请输入数字"));
      }
      return true;
    };
    return {
      formData: {
        symbol: this.row.symbol,
        desc: this.row.desc,
        data: this.row.data?this.row.data:0,
        dataValue: ""
      },
      sheetField: global.sheetField,
      rules: {
        dataValue: [
          { required: true, message: "请输入设置值", trigger: "blur" },
          { validator: checkNum, trigger: "blur" }
        ]
      }
    };
  },
  methods: {
    // 提交表单
    onSubmit(form) {
      // this.$store.dispatch("addNewRule", { ...this.formData, status: true });
      this.$refs[form].validate(valid => {
        if (valid) {
          var params = {
            value: this.formData.dataValue
          };
          this.$store.dispatch("getTableData", params);
          this.closeDialog(form);
          this.onClose();
        } else {
          return false;
        }
      });
    },
    resetForm(formName) {
      this.$refs[formName].resetFields()
      this.$refs[formName].clearValidate()
      this.onClose()
    }
  }
};
</script>