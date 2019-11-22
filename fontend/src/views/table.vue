<template>
  <el-container>
    <el-header style="text-align:center">
      <h1>功能参数列表</h1>
    </el-header>

    <el-main>
      <el-button round type="primary" @click="addEvent()">添加</el-button>
      <el-button round type="primary" @click="temperature()">温度曲线图</el-button>
      <el-table
        :data="getRules"
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
          </template>
        </el-table-column>
      </el-table>
    </el-main>
  </el-container>
</template>

<script>
import global from "./constant";
import { mapGetters } from "vuex";
export default {
  computed: {
    ...mapGetters(["getRules"])
  },

  methods: {
    addEvent() {
      this.$router.push({ path: "/addEvent" });
    },
    temperature() {
      this.$router.push({ path: "/temperature" });
    }
  },

  data() {
    return {
      sheetConfig: global.sheetField
    };
  },

  created() {},

  mounted() {}
};
</script>