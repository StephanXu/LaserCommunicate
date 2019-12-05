<template>
  <el-container class="table-page">
    <el-header>
      <el-row>
        <el-col :span="8">
          <h2>功能参数列表</h2>
        </el-col>
        <el-col :span="16">
          <div class="table-control-button-container">
            <el-select style="margin-right:5px" v-model="portValue" placeholder="请选择串口">
              <el-option
                v-for="item in portOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value"
              ></el-option>
            </el-select>
            <el-select v-model="styleValue" placeholder="请选择模式" @change="chooseStyle(value)">
              <el-option
                v-for="item in styleOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value"
              ></el-option>
            </el-select>
            <el-button
              class="connectButton"
              v-if="!getConnectStatus"
              @click="initConnect()"
              icon="el-icon-circle-check"
            >连接</el-button>
            <el-button
              class="connectButton"
              v-else
              @click="disconnect()"
              icon="el-icon-circle-close"
            >断开连接</el-button>
          </div>
        </el-col>
      </el-row>
    </el-header>
    <el-main>
      <el-table 
      :data="getTable.filter(data => !search || data.desc.toLowerCase().includes(search.toLowerCase()))"
       ref="multipleTable" tooltip-effect="dark" height="550">
        <el-table-column
          v-for="item in sheetConfig"
          :key="item.propName"
          :prop="item.propName"
          :label="item.text"
        ></el-table-column>
        <el-table-column align="right" width="200">
          <template slot="header">
            <el-input v-model="search" size="medium" placeholder="输入关键字搜索"/>
          </template>
          <template slot-scope="scope">
            <el-button
              v-if="writable(scope.row)"
              type="success"
              size="mini"
              @click="handleCurrentChange(scope.row)"
            >发送</el-button>
            <el-button v-else type="danger" size="mini" disabled>只读</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-main>

    <el-dialog title="发送设置" :visible.sync="isAddRuleVisile" width="60%" @close="handleClose">
      <add-rule ref="addRule" :row="message" :on-close="handleAddRule"></add-rule>
    </el-dialog>
  </el-container>
</template>

<script>
import global from "./constant";
import { mapGetters } from "vuex";
import AddRule from "./addRule";

export default {
  components: {
    "add-rule": AddRule
  },
  computed: {
    ...mapGetters(["getTable"]),
    ...mapGetters(["getConnectStatus"]),
  },
  methods: {
    // 断开连接
    disconnect() {
      var value = this.portValue;
      this.$store.dispatch("disConnect", value);
      this.$store.dispatch("getTableData", { mode: this.styleValue });
      this.portValue = "";
    },
    // 先选择串口，然后连接
    initConnect() {
      var value = this.portValue;
      if (value == "") {
        this.$message.warning("请先选择串口！");
      } else {
        this.$store.dispatch("initConnect", value);
        this.$store.dispatch("getTableData", { mode: this.styleValue });
      }
    },
    // 选择模式
    chooseStyle(value) {
      var params = {
        mode: value
      };
      this.$store.dispatch("getTableData", params);
    },
    // 判断是否可写
    writable(row) {
      if (row.writable === "true") {
        return true;
      } else {
        return false;
      }
    },
    // 发送参数
    handleCurrentChange(currentRow) {
      if (currentRow.writable === "true") {
        this.message = currentRow;
        this.isAddRuleVisile = true;
      }
    },
    // 关闭弹窗的回调
    handleClose() {
      this.$refs.addRule.resetForm("formData");
    },
    handleAddRule() {
      this.isAddRuleVisile = false;
    }
  },

  data() {
    return {
      sheetConfig: global.sheetField,
      selected: {},
      isAddRuleVisile: false,
      search: '',
      message: {},
      styleOptions: [
        {
          value: "0",
          label: "通用模式"
        },
        {
          value: "1",
          label: "温度反馈模式"
        },
        {
          value: "2",
          label: "光功率反馈模式"
        },
        {
          value: "3",
          label: "电功率反馈模式"
        }
      ],
      styleValue: "0",
      portOptions: [
        {
          value: "\\\\.\\COM1",
          label: "COM1"
        },
        {
          value: "\\\\.\\COM2",
          label: "COM2"
        },
        {
          value: "\\\\.\\COM3",
          label: "COM3"
        },
        {
          value: "\\\\.\\COM4",
          label: "COM4"
        },
        {
          value: "\\\\.\\COM5",
          label: "COM5"
        },
        {
          value: "\\\\.\\COM6",
          label: "COM6"
        },
        {
          value: "\\\\.\\COM7",
          label: "COM7"
        },
        {
          value: "\\\\.\\COM8",
          label: "COM8"
        },
        {
          value: "\\\\.\\COM9",
          label: "COM9"
        },
        {
          value: "\\\\.\\COM10",
          label: "COM10"
        }
      ],
      portValue: ""
    };
  }
};
</script>

<style scoped>
.table-page {
  background-color: #fff;
}

.table-control-button-container {
  display: flex;
  justify-content: flex-end;
  align-items: center;
  height: 75px;
}
</style>