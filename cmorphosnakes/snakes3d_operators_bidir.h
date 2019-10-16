// flat surfaces
bool ISd_3d_0_bidir(uint8_t const *cont_, int index_, int nsi_, int si_, int nsj_,
                    int sj_, int nsk_, int sk_, int value) {
  return (!(cont_[index_ - nsj_ - nsk_] == value || cont_[index_ - nsj_] == value ||
            cont_[index_ - nsj_ + sk_] == value || cont_[index_ - nsk_] == value ||
            cont_[index_] == value || cont_[index_ + sk_] == value ||
            cont_[index_ + sj_ - nsk_] == value || cont_[index_ + sj_] == value ||
            cont_[index_ + sj_ + sk_] == value));
}
bool ISd_3d_1_bidir(uint8_t const *cont_, int index_, int nsi_, int si_, int nsj_,
                    int sj_, int nsk_, int sk_, int value) {
  return (!(cont_[index_ - nsi_ - nsk_] == value || cont_[index_ - nsi_] == value ||
            cont_[index_ - nsi_ + sk_] == value || cont_[index_ - nsk_] == value ||
            cont_[index_] == value || cont_[index_ + sk_] == value ||
            cont_[index_ + si_ - nsk_] == value || cont_[index_ + si_] == value ||
            cont_[index_ + si_ + sk_] == value));
}
bool ISd_3d_2_bidir(uint8_t const *cont_, int index_, int nsi_, int si_, int nsj_,
                    int sj_, int nsk_, int sk_, int value) {
  return (!(cont_[index_ - nsi_ - nsj_] == value || cont_[index_ - nsi_] == value ||
            cont_[index_ - nsi_ + sj_] == value || cont_[index_ - nsj_] == value ||
            cont_[index_] == value || cont_[index_ + sj_] == value ||
            cont_[index_ + si_ - nsj_] == value || cont_[index_ + si_] == value ||
            cont_[index_ + si_ + sj_] == value));
}
// diagonals loop i
bool ISd_3d_3_bidir(uint8_t const *cont_, int index_, int nsi_, int si_, int nsj_,
                    int sj_, int nsk_, int sk_, int value) {
  return (!(cont_[index_ - nsi_ - nsj_ - nsk_] == value ||
            cont_[index_ - nsi_] == value ||
            cont_[index_ - nsi_ + sj_ + sk_] == value ||
            cont_[index_ - nsj_ - nsk_] == value || cont_[index_] == value ||
            cont_[index_ + sj_ + sk_] == value ||
            cont_[index_ + si_ - nsj_ - nsk_] == value ||
            cont_[index_ + si_] == value || cont_[index_ + si_ + sj_ + sk_] == value));
}
bool ISd_3d_4_bidir(uint8_t const *cont_, int index_, int nsi_, int si_, int nsj_,
                    int sj_, int nsk_, int sk_, int value) {
  return (!(cont_[index_ - nsi_ - nsj_ + sk_] == value ||
            cont_[index_ - nsi_] == value ||
            cont_[index_ - nsi_ + sj_ - nsk_] == value ||
            cont_[index_ - nsj_ + sk_] == value || cont_[index_] == value ||
            cont_[index_ + sj_ - nsk_] == value ||
            cont_[index_ + si_ - nsj_ + sk_] == value || cont_[index_ + si_] == value ||
            cont_[index_ + si_ + sj_ - nsk_] == value));
}
// diagonals loop j
bool ISd_3d_5_bidir(uint8_t const *cont_, int index_, int nsi_, int si_, int nsj_,
                    int sj_, int nsk_, int sk_, int value) {
  return (!(cont_[index_ - nsi_ - nsj_ - nsk_] == value ||
            cont_[index_ - nsj_] == value ||
            cont_[index_ + si_ - nsj_ + sk_] == value ||
            cont_[index_ - nsi_ - nsk_] == value || cont_[index_] == value ||
            cont_[index_ + si_ + sk_] == value ||
            cont_[index_ - nsi_ + sj_ - nsk_] == value ||
            cont_[index_ + sj_] == value || cont_[index_ + si_ + sj_ + sk_] == value));
}
bool ISd_3d_6_bidir(uint8_t const *cont_, int index_, int nsi_, int si_, int nsj_,
                    int sj_, int nsk_, int sk_, int value) {
  return (!(cont_[index_ - nsi_ - nsj_ + sk_] == value ||
            cont_[index_ - nsj_] == value ||
            cont_[index_ + si_ - nsj_ - nsk_] == value ||
            cont_[index_ - nsi_ + sk_] == value || cont_[index_] == value ||
            cont_[index_ + si_ - nsk_] == value ||
            cont_[index_ - nsi_ + sj_ + sk_] == value || cont_[index_ + sj_] == value ||
            cont_[index_ + si_ + sj_ - nsk_] == value));
}
// diagonals loop k
bool ISd_3d_7_bidir(uint8_t const *cont_, int index_, int nsi_, int si_, int nsj_,
                    int sj_, int nsk_, int sk_, int value) {
  return (!(cont_[index_ - nsi_ - nsj_ - nsk_] == value ||
            cont_[index_ - nsk_] == value ||
            cont_[index_ + si_ + sj_ - nsk_] == value ||
            cont_[index_ - nsi_ - nsj_] == value || cont_[index_] == value ||
            cont_[index_ + si_ + sj_] == value ||
            cont_[index_ - nsi_ - nsj_ + sk_] == value ||
            cont_[index_ + sk_] == value || cont_[index_ + si_ + sj_ + sk_] == value));
}
bool ISd_3d_8_bidir(uint8_t const *cont_, int index_, int nsi_, int si_, int nsj_,
                    int sj_, int nsk_, int sk_, int value) {
  return (!(cont_[index_ - nsi_ + sj_ - nsk_] == value ||
            cont_[index_ - nsk_] == value ||
            cont_[index_ + si_ - nsj_ - nsk_] == value ||
            cont_[index_ - nsi_ + sj_] == value || cont_[index_] == value ||
            cont_[index_ + si_ - nsj_] == value ||
            cont_[index_ - nsi_ + sj_ + sk_] == value || cont_[index_ + sk_] == value ||
            cont_[index_ + si_ - nsj_ + sk_] == value));
}
bool ISd_3d_any_bidir(uint8_t const *cont_, int index_, int nsi_, int si_, int nsj_,
                      int sj_, int nsk_, int sk_, int value) {
  return (!(ISd_3d_0_bidir(cont_, index_, nsi_, si_, nsj_, sj_, nsk_, sk_, value) ||
            ISd_3d_1_bidir(cont_, index_, nsi_, si_, nsj_, sj_, nsk_, sk_, value) ||
            ISd_3d_2_bidir(cont_, index_, nsi_, si_, nsj_, sj_, nsk_, sk_, value) ||
            ISd_3d_3_bidir(cont_, index_, nsi_, si_, nsj_, sj_, nsk_, sk_, value) ||
            ISd_3d_4_bidir(cont_, index_, nsi_, si_, nsj_, sj_, nsk_, sk_, value) ||
            ISd_3d_5_bidir(cont_, index_, nsi_, si_, nsj_, sj_, nsk_, sk_, value) ||
            ISd_3d_6_bidir(cont_, index_, nsi_, si_, nsj_, sj_, nsk_, sk_, value) ||
            ISd_3d_7_bidir(cont_, index_, nsi_, si_, nsj_, sj_, nsk_, sk_, value) ||
            ISd_3d_8_bidir(cont_, index_, nsi_, si_, nsj_, sj_, nsk_, sk_, value)));
}
