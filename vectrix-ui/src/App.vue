<template>
  <div>
    <div id="app" ref="spreadsheet"></div>
    <div>
      <input type="button" value="Add new row" @click="() => spreadsheet.insertRow()" />
    </div>
  </div>
</template>

<script>
import jexcel from "jexcel";
import "jexcel/dist/jexcel.css";
import axios from "axios";

let failed_updates = [];

let changed = function (instance, cell, col, row, value) {
  console.log(`[${col}, ${row}] = ${value}`);
  axios
    .post(`http://localhost:8000/set/float/example/${col}/${row}/${value}`, {})
    .then((response) => console.log(response))
    .catch((error) => {
      console.log(error);
      console.log("all failed_updates:", failed_updates);
      failed_updates.push({
        col: col,
        row: row,
        value: value,
      });
    });
};

let data = [
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
  ["", "", "", "", "", "", ""],
];

let options = {
  data: data,
  allowToolbar: true,
  onchange: changed,
  columns: [
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
    {
      width: "120px",
    },
  ],
};

export default {
  name: "App",
  mounted: function () {
    let spreadsheet = jexcel(this.$el, options);
    Object.assign(this, { spreadsheet });
  },
};
</script>