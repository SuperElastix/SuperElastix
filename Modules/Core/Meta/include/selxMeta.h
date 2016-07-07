template <typename...> struct TypeList;

template <typename, typename> struct list_append;

template <typename... Ts, typename... Us>
struct list_append<TypeList<Ts...>, TypeList<Us...>> {
  using type = TypeList<Ts..., Us...>;
};
