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

let table = "example";
let failed_updates = {};

let is_number = (n) => {
  return !isNaN(n);
};

let handle_change = (col, row, value) => {
  const path =
    value === ""
      ? `http://localhost:8000/clear/${table}/${col}/${row}`
      : is_number(value)
      ? `http://localhost:8000/set/float/${table}/${col}/${row}/${value}`
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
  handle_change(col, row, value);
  retry_updates();
};

/* eslint-disable no-unused-vars */
let selectionActive = (instance, x1, y1, x2, y2, origin) => {
  load(instance.jexcel, x1, y1);
};
/* eslint-enable no-unused-vars */

let load = (spreadsheet, x, y) => {
  const start_x = x - 10 > 0 ? x - 10 : 0;
  const start_y = y - 10 > 0 ? y - 10 : 0;
  for (let col = start_x; col < x + 10; col++) {
    for (let row = start_y; row < y + 10; row++) {
      axios
        .get(`http://localhost:8000/get/type/${table}/${col}/${row}`)
        .then((response) => {
          const type = response.data;
          let url = null;
          switch (type) {
            case "Nil":
              break;

            case "Float":
              url = `http://localhost:8000/get/float/${table}/${col}/${row}`;
              break;

            case "Int":
              url = `http://localhost:8000/get/int/${table}/${col}/${row}`;
              break;

            default:
              break;
          }

          if (url) {
            axios.get(url).then((rsp) => {
              spreadsheet.setValue(
                col_index_to_name(col) + (row + 1),
                rsp.data,
                true
              );
            });
          }
        })
        .catch((err) => console.log(err));
    }
  }
};

let data = [];

let options = {
  data: data,
  allowToolbar: true,
  onchange: changed,
  onselection: selectionActive,
  lazyLoading: true,
  tableOverflow: true,
  fullscreen: true,
  minDimensions: [48, 1000],
  colWidths: [120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120],
  columns: [],
};

export default {
  name: "App",
  mounted: function () {
    let spreadsheet = jexcel(this.$el, options);
    Object.assign(this, { spreadsheet });
    load(spreadsheet, 0, 0);
    window.setInterval(() => {
      retry_updates();
    }, 60000);
  },
};
</script>