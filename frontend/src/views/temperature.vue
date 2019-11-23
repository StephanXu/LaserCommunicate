<template>
  <el-container>
    <el-header>
      <el-page-header @back="goBack" content="温度变化曲线" style="margin-top:20px"></el-page-header>
    </el-header>
    <el-main>
      <div style="align:center;">
        <div id="myChart" :style="{width: '95vw', height: '60vh'}"></div>
      </div>
    </el-main>
  </el-container>
</template>
<script>
import { setInterval } from "timers";
export default {
  data() {
    return {
      option: {
        xAxis: {
          type: "category",
          data: [
            "Mon",
            "Tue",
            "Wed",
            "Thu",
            "Fri",
            "Sat",
            "Sun",
            "Mon",
            "Tue",
            "Wed",
            "Thu",
            "Fri",
            "Sat",
            "Sun",
            "Mon",
            "Tue",
            "Wed",
            "Thu",
            "Fri",
            "Sat",
            "Sun",
            "Mon",
            "Tue",
            "Wed",
            "Thu",
            "Fri",
            "Sat",
            "Sun"
          ]
        },
        yAxis: {
          type: "value"
        },
        series: [
          {
            data: [
              1820,
              1932,
              1901,
              1934,
              1290,
              1330,
              1320,
              820,
              932,
              901,
              934,
              1290,
              1330,
              1320,
              820,
              932,
              901,
              934,
              1290,
              1330,
              1320,
              820,
              932,
              901,
              934,
              1290,
              1330,
              1320
            ],
            type: "line",
            smooth: true
          }
        ]
      }
    };
  },
  mounted() {
    this.drawLine();
  },
  methods: {
    drawLine() {
      // 基于准备好的dom，初始化echarts实例
      let myChart = this.$echarts.init(document.getElementById("myChart"));
      // 绘制图表
      myChart.setOption(this.option);
      let temdata = this.option.series[0].data;
      setInterval(() => {
        var a = temdata.splice(0, 1);
        temdata.push(a[0]);
        myChart.setOption(this.option);
      }, 500);
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
