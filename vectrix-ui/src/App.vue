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

let failed_updates = {};

let is_number = (n) => {
  return !isNaN(n);
};

let handle_change = (col, row, value) => {
  console.log(`[${col},${row}] = ${value}::${typeof value}`);
  const path =
    value === ""
      ? `http://localhost:8000/clear/example/${col}/${row}`
      : is_number(value)
      ? `http://localhost:8000/set/float/example/${col}/${row}/${value}`
      : ""; /* TODO: how to add strings? */

  if (path !== "") {
    const key = `${col},${row}`;
    axios
      .post(path, {})
      .then((response) => {
        console.log(`successfully updated [${key}] = ${value}`);
        console.log(response);
        delete failed_updates[key];
      })
      .catch((error) => {
        console.log(error);
        failed_updates[key] = value;
      });
  }
};

let retry_updates = () => {
  Object.keys(failed_updates).map((key) => {
    const v = failed_updates[key];
    const [c, r] = key.split(",");
    console.log(`retrying: ${c} ${r} ${v}`);
    handle_change(c, r, v);
  });
};

let changed = (instance, cell, col, row, value) => {
  handle_change(col, row, value);
  retry_updates();
};

let data = [];

let options = {
  data: data,
  allowToolbar: true,
  onchange: changed,
  lazyLoading: true,
  tableOverflow: true,
  fullscreen: true,
  minDimensions: [48, 1000],
  columns: [],
};

export default {
  name: "App",
  mounted: function () {
    /* TODO: load data from DB on mount */
    let spreadsheet = jexcel(this.$el, options);
    Object.assign(this, { spreadsheet });
    window.setInterval(() => {
      retry_updates();
    }, 10000);
  },
};
</script>