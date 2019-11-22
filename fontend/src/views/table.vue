<template>
  <el-container>
    <el-header style="text-align:center">
      <!-- <font color="green" size="8" font-family="微软雅黑" >功能参数列表</font> -->
      <h1>功能参数列表</h1>
    </el-header>

    <el-main>
      <el-button round type="primary" @click="addevent()">添加</el-button>
      <el-button round type="primary" @click="temperature()">温度曲线图</el-button>
      <el-table
        :data="tableData"
        border
        stripe
        style="margin-top:5px"
        ref="multipleTable"
        tooltip-effect="dark"
      >
        <el-table-column
          v-for="item in sheetConfig"
          :key="item.propName"
          :prop="item.propName"
          :label="item.text"
        ></el-table-column>
        <el-table-column label="状态" width="50">
          <template slot-scope="scope">
            <el-button
              v-if="scope.row.status"
              type="success"
              icon="el-icon-check"
              size="mini"
              circle
            ></el-button>
            <el-button v-else type="danger" icon="el-icon-close" size="mini" circle></el-button>
            <!-- <el-button @click="deleteevent(scope.row)" type="text" size="small">删除</el-button> -->
            <!-- <el-button @click="editevent(scope.row)" type="text" size="small">发送</el-button> -->
          </template>
        </el-table-column>
      </el-table>
    </el-main>
  </el-container>
</template>

<script>
import global from "./constant";

export default {
  methods: {
    deleteevent(row) {
      console.log(row);
    },
    editevent(row) {
      console.log(row);
    },
    addevent() {
      this.$router.push({
        path: "/addevent",
        query: {}
      });
    },
    temperature() {
      this.$router.push({
        path: "/temperature",
        query: {}
      });
    },

    saveConfig() {
      this.$ipcRenderer.sendSync(
        "saveConfigs",
        JSON.stringify({
          func: this.tableData
        })
      );
    }
  },

  data() {
    return {
      sheetConfig: global.sheetField,
      tableData: []
    };
  },

  created() {
    // this.$ipcRenderer.on("saveStatus", (event, arg) => {
    //   if (arg === "fail") {
    //     // vm.msgbox("错误", "保存设置失败");
    //     alert("保存设置失败");
    //   }
    // });

    console.log("fuck");
    if (this.$route.query.newData != undefined) {
      this.tableData.push({
        alias: this.$route.query.newData[0].content,
        regOffset: this.$route.query.newData[1].content,
        funcCode: this.$route.query.newData[2].content,
        dataType: this.$route.query.newData[3].content,
        desc: this.$route.query.newData[4].content,
        status: true
      });
      this.saveConfig();
    }

    let allConfigs = JSON.parse(this.$ipcRenderer.sendSync("loadConfigs", ""));
    this.tableData = allConfigs.func;
  },

  mounted() {}
};
</script>