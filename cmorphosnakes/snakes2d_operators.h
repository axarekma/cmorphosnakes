// diagonal
bool SId_2d_0(uint8_t const *cont_, int idx_, int si_, int sj_, int value) {
  return (cont_[idx_ - si_ - sj_] == value && cont_[idx_] == value &&
          cont_[idx_ + si_ + sj_] == value);
}
bool ISd_2d_0(uint8_t const *cont_, int idx_, int si_, int sj_, int value) {
  return (!(cont_[idx_ - si_ - sj_] == value || cont_[idx_] == value ||
            cont_[idx_ + si_ + sj_] == value));
}

// const y
bool SId_2d_1(uint8_t const *cont_, int idx_, int si_, int sj_, int value) {
  return (cont_[idx_ - si_] == value && cont_[idx_] == value &&
          cont_[idx_ + si_] == value);
}
bool ISd_2d_1(uint8_t const *cont_, int idx_, int si_, int sj_, int value) {
  return (!(cont_[idx_ - si_] == value || cont_[idx_] == value ||
            cont_[idx_ + si_] == value));
}

// diagonal
bool SId_2d_2(uint8_t const *cont_, int idx_, int si_, int sj_, int value) {
  return (cont_[idx_ - si_ + sj_] == value && cont_[idx_] == value &&
          cont_[idx_ + si_ - sj_] == value);
}
bool ISd_2d_2(uint8_t const *cont_, int idx_, int si_, int sj_, int value) {
  return (!(cont_[idx_ - si_ + sj_] == value || cont_[idx_] == value ||
            cont_[idx_ + si_ - sj_] == value));
}

// const x
bool SId_2d_3(uint8_t const *cont_, int idx_, int si_, int sj_, int value) {
  return (cont_[idx_ - sj_] == value && cont_[idx_] == value &&
          cont_[idx_ + sj_] == value);
}
bool ISd_2d_3(uint8_t const *cont_, int idx_, int si_, int sj_, int value) {
  return (!(cont_[idx_ - sj_] == value || cont_[idx_] == value ||
            cont_[idx_ + sj_] == value));
}

bool SId_2d_any(uint8_t const *cont_, int idx_, int si_, int sj_, int value) {
  return (SId_2d_0(cont_, idx_, si_, sj_, value) ||
          SId_2d_1(cont_, idx_, si_, sj_, value) ||
          SId_2d_2(cont_, idx_, si_, sj_, value) ||
          SId_2d_3(cont_, idx_, si_, sj_, value));
}

bool ISd_2d_any(uint8_t const *cont_, int idx_, int si_, int sj_, int value) {
  return (!(ISd_2d_0(cont_, idx_, si_, sj_, value) ||
            ISd_2d_1(cont_, idx_, si_, sj_, value) ||
            ISd_2d_2(cont_, idx_, si_, sj_, value) ||
            ISd_2d_3(cont_, idx_, si_, sj_, value)));
}
