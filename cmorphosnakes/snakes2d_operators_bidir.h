// diagonal
bool ISd_2d_0_bidir(uint8_t const *cont_, int idx_, int nsi_, int si_, int nsj_,
                    int sj_, int value) {
  return (!(cont_[idx_ - nsi_ - nsj_] == value || cont_[idx_] == value ||
            cont_[idx_ + si_ + sj_] == 1));
}

// const y
bool ISd_2d_1_bidir(uint8_t const *cont_, int idx_, int nsi_, int si_, int nsj_,
                    int sj_, int value) {
  return (!(cont_[idx_ - nsi_] == value || cont_[idx_] == value ||
            cont_[idx_ + si_] == value));
}

// diagonal
bool ISd_2d_2_bidir(uint8_t const *cont_, int idx_, int nsi_, int si_, int nsj_,
                    int sj_, int value) {
  return (!(cont_[idx_ - nsi_ + sj_] == value || cont_[idx_] == value ||
            cont_[idx_ + si_ - nsj_] == value));
}

// const x
bool ISd_2d_3_bidir(uint8_t const *cont_, int idx_, int nsi_, int si_, int nsj_,
                    int sj_, int value) {
  return (!(cont_[idx_ - nsj_] == value || cont_[idx_] == value ||
            cont_[idx_ + sj_] == value));
}

bool ISd_2d_any_bidir(uint8_t const *cont_, int idx_, int nsi_, int si_, int nsj_,
                      int sj_, int value) {
  return (!(ISd_2d_0_bidir(cont_, idx_, nsi_, si_, nsj_, sj_, value) ||
            ISd_2d_1_bidir(cont_, idx_, nsi_, si_, nsj_, sj_, value) ||
            ISd_2d_2_bidir(cont_, idx_, nsi_, si_, nsj_, sj_, value) ||
            ISd_2d_3_bidir(cont_, idx_, nsi_, si_, nsj_, sj_, value)));
}
