type char = reg;

type or_t2 = <
  word w e;
  char r;
>;

type and_t = ?[ word w e; reg r;];
type and_t_singular = @##[ word w;];

type nested_t = [
  [word _g e; reg _q;] e f;
  reg r;
];

type main_t = (reg):reg;

main_t main = (x) {
  return 0;
};
