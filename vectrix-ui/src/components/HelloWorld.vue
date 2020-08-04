<template>
  <div>
    <table style="width:100%">
      <tr>
        <th>Firstname</th>
        <th>Lastname</th>
        <th>Age</th>
      </tr>
      <tr>
        <td contenteditable="true">Jill</td>
        <td contenteditable="true">Smith</td>
        <td contenteditable="true">50</td>
      </tr>
      <tr>
        <td contenteditable="true">Eve</td>
        <td contenteditable="true">Jackson</td>
        <td contenteditable="true">94</td>
      </tr>
    </table>

    <table style="width:100%">
      <tr>
        <th>Name</th>
        <th colspan="2">Telephone</th>
      </tr>
      <tr>
        <td contenteditable="true">Bill Gates</td>
        <td contenteditable="true">55577854</td>
        <td contenteditable="true">55577855</td>
      </tr>
    </table>

    <table style="width:100%">
      <tr>
        <th>Name</th>
        <th colspan="2">Telephone</th>
      </tr>
      <tr>
        <td contenteditable="true">Bill Gates</td>
        <td contenteditable="true">55577854</td>
        <td contenteditable="true">55577855</td>
      </tr>
    </table>

    <table style="width:100%">
      <caption>Monthly savings</caption>
      <tr>
        <th>Month</th>
        <th>Savings</th>
      </tr>
      <tr>
        <td contenteditable="true">January</td>
        <td contenteditable="true">$100</td>
      </tr>
      <tr>
        <td contenteditable="true">February</td>
        <td contenteditable="true">$50</td>
      </tr>
    </table>

    <table id="t01">
      <tr>
        <th>Firstname</th>
        <th>Lastname</th>
        <th>Age</th>
      </tr>
      <tr>
        <td contenteditable="true">Eve</td>
        <td contenteditable="true">Jackson</td>
        <td contenteditable="true">94</td>
      </tr>
    </table>
  </div>
</template>

<script>
export default {
  name: "HelloWorld",
  props: {
    msg: String,
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
table,
th,
td {
  border: 1px solid grey;
  border-collapse: collapse;
}

th,
td {
  padding: 2px;
}

th {
  text-align: left;
}

#t01 {
  width: 100%;
  background-color: #f1f1c1;
}

#t01 tr:nth-child(even) {
  background-color: #eee;
}
#t01 tr:nth-child(odd) {
  background-color: #fff;
}
#t01 th {
  color: white;
  background-color: black;
}
</style>
