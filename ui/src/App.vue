<template>
  <div>
    <div id="app" ref="spreadsheet"></div>
    <div>
      <input type="button" value="Add new row" @click="() => spreadsheet.insertRow()" />
    </div>
    <div>
      <input v-model="table" placeholder="edit me" />
      <p>Table: {{ table }}</p>
    </div>
  </div>
</template>

<script>
import jexcel from "jexcel";
import "jexcel/dist/jexcel.css";
import axios from "axios";

let table = "example";
let host = "192.168.0.109:8000";
let failed_updates = {};

let ref_x = 0;
let ref_y = 0;

let is_number = (n) => {
  return !isNaN(n);
};

let handle_change = (col, row, value) => {
  const path =
    value === ""
      ? `http://${host}/clear/${table}/${col}/${row}`
      : is_number(value)
      ? `http://${host}/set/float/${table}/${col}/${row}/${value}`
      : ""; /* TODO: how to add strings? */

  if (path !== "") {
    const key = `${col},${row}`;
    axios
      .post(path, {})
      .then(() => {
        delete failed_updates[key];
      })
      .catch((error) => {
        console.log(error);
        failed_updates[key] = value;
      });
  }
};

let col_index_to_name = (column_index) => {
  let dividend = column_index + 1;
  let column_name = "";

  while (dividend > 0) {
    let modulo = (dividend - 1) % 26;
    column_name = String.fromCharCode(65 + modulo) + column_name;
    dividend = Math.floor((dividend - modulo) / 26);
  }

  return column_name;
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
  if (col == ref_x && row == ref_y) {
    handle_change(col, row, value);
  }
};

const load_chunk = 40;

/* eslint-disable no-unused-vars */
let selectionActive = (instance, x1, y1, x2, y2, origin) => {
  ref_x = x1;
  ref_y = y1;
  load(instance.jexcel, x1, y1);
  /*
  console.log(instance.max_y, y1, instance.max_x, x1);
  if (instance.max_y < y1 || instance.max_x < x1) {
    load(instance.jexcel, x1, y1);
    instance.max_y = Math.max(y1 + load_chunk, instance.max_y);
    instance.max_x = Math.max(x1 + load_chunk, instance.max_x);
  }
  if (isNaN(instance.max_y)) {
    instance.max_y = 0;
  }

  if (isNaN(instance.max_x)) {
    instance.max_x = 0;
  }
  */
};
/* eslint-enable no-unused-vars */

/* eslint-disable no-unused-vars */
let focus = (instance) => {
  console.log("focus");
};
/* eslint-enable no-unused-vars */

let load = (spreadsheet, x, y) => {
  const start_x = x - load_chunk > 0 ? x - load_chunk : 0;
  const start_y = y - load_chunk > 0 ? y - load_chunk : 0;
  const end_x = x + load_chunk;
  const end_y = y + load_chunk;
  const url = `http://${host}/get/range/${table}/${start_x}/${end_x}/${start_y}/${end_y}`;

  axios
    .get(url)
    .then((response) => {
      response.data.forEach((cell) => {
        const cell_name = col_index_to_name(cell.col) + (cell.row + 1);
        const will_set_value =
          spreadsheet.getValue(cell_name) !== cell.data || cell.data === "";
        if (will_set_value) {
          spreadsheet.setValue(cell_name, cell.data, true);
        }
      });
    })
    .catch((err) => console.log(err));
};

let data = [];

let options = {
  data: data,
  allowToolbar: true,
  onchange: changed,
  onselection: selectionActive,
  onfocus: focus,
  minDimensions: [48, 1000],
  colWidths: [120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120],
  columns: [],
};

export default {
  name: "App",
  mounted() {
    let spreadsheet = jexcel(this.$el, options);
    Object.assign(this, { spreadsheet });
    load(spreadsheet, 0, 0);
    window.setInterval(() => {
      load(spreadsheet, ref_x, ref_y);
      retry_updates();
    }, 10000);
  },
};
</script>
