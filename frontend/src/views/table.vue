<template>
  <el-container class="table-page">
    <el-header>
      <el-row>
        <el-col :span="12">
          <h2>功能参数列表</h2>
        </el-col>
        <el-col :span="12">
          <div class="table-control-button-container">
            <el-button round @click="addRule()" icon="el-icon-plus">添加</el-button>
            <el-button round @click="deleteEvent()" icon="el-icon-plus">删除</el-button>
            <el-button round @click="temperature()">温度曲线图</el-button>
          </div>
        </el-col>
      </el-row>
    </el-header>
    <el-main>
      <el-table
        :data="getRules"
        border
        stripe
        highlight-current-row
        @current-change="handleCurrentChange"
        style="margin-top:5px;box-shadow: 0 2px 4px rgba(0, 0, 0, .12), 0 0 6px rgba(0, 0, 0, .04)"
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

    <el-drawer
      title="添加规则"
      :show-close="false"
      :visible.sync="isAddRuleVisile"
      direction="rtl"
      size="50%"
    >
      <add-rule :on-close="handleAddRule"></add-rule>
    </el-drawer>
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
    ...mapGetters(["getRules"])
  },

  methods: {
    addRule() {
      this.isAddRuleVisile = true;
    },
    deleteEvent() {
      this.$store.dispatch("removeRule", this.selected);
    },
    temperature() {
      this.$router.push({ path: "/temperature" });
    },
    handleCurrentChange(currentRow) {
      this.selected = currentRow;
    },
    handleAddRule() {
      this.isAddRuleVisile = false;
      console.log("got it");
    }
  },

  data() {
    return {
      sheetConfig: global.sheetField,
      selected: {},
      isAddRuleVisile: false
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