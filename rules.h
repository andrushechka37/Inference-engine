if (variables->data[0].value) {
variables->data[0].value = 0;
variables->data[1].value = 1;

is_change = 1;
fprintf(report, "the rule worked");
}
if(variables->data[1].value) {
variables->data[2].value = 0;
variables->data[1].value = 0;
variables->data[0].value = 0;

is_change = 1;
fprintf(report, "the rule worked");
}
