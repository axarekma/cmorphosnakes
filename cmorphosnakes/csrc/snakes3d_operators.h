// flat surfaces
bool SId_3d_0(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (cont_[index_ - sj_ - sk_] == value && cont_[index_ - sj_] == value &&
          cont_[index_ - sj_ + sk_] == value && cont_[index_ - sk_] == value &&
          cont_[index_] == value && cont_[index_ + sk_] == value &&
          cont_[index_ + sj_ - sk_] == value && cont_[index_ + sj_] == value &&
          cont_[index_ + sj_ + sk_] == value);
}

bool SId_3d_1(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (cont_[index_ - si_ - sk_] == value && cont_[index_ - si_] == value &&
          cont_[index_ - si_ + sk_] == value && cont_[index_ - sk_] == value &&
          cont_[index_] == value && cont_[index_ + sk_] == value &&
          cont_[index_ + si_ - sk_] == value && cont_[index_ + si_] == value &&
          cont_[index_ + si_ + sk_] == value);
}

bool SId_3d_2(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (cont_[index_ - si_ - sj_] == value && cont_[index_ - si_] == value &&
          cont_[index_ - si_ + sj_] == value && cont_[index_ - sj_] == value &&
          cont_[index_] == value && cont_[index_ + sj_] == value &&
          cont_[index_ + si_ - sj_] == value && cont_[index_ + si_] == value &&
          cont_[index_ + si_ + sj_] == value);
}

// diagonals loop i
bool SId_3d_3(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (cont_[index_ - si_ - sj_ - sk_] == value && cont_[index_ - si_] == value &&
          cont_[index_ - si_ + sj_ + sk_] == value &&
          cont_[index_ - sj_ - sk_] == value && cont_[index_] == value &&
          cont_[index_ + sj_ + sk_] == value &&
          cont_[index_ + si_ - sj_ - sk_] == value && cont_[index_ + si_] == value &&
          cont_[index_ + si_ + sj_ + sk_] == value);
}

bool SId_3d_4(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (cont_[index_ - si_ - sj_ + sk_] == value && cont_[index_ - si_] == value &&
          cont_[index_ - si_ + sj_ - sk_] == value &&
          cont_[index_ - sj_ + sk_] == value && cont_[index_] == value &&
          cont_[index_ + sj_ - sk_] == value &&
          cont_[index_ + si_ - sj_ + sk_] == value && cont_[index_ + si_] == value &&
          cont_[index_ + si_ + sj_ - sk_] == value);
}

// diagonals loop j_
bool SId_3d_5(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (cont_[index_ - si_ - sj_ - sk_] == value && cont_[index_ - sj_] == value &&
          cont_[index_ + si_ - sj_ + sk_] == value &&
          cont_[index_ - si_ - sk_] == value && cont_[index_] == value &&
          cont_[index_ + si_ + sk_] == value &&
          cont_[index_ - si_ + sj_ - sk_] == value && cont_[index_ + sj_] == value &&
          cont_[index_ + si_ + sj_ + sk_] == value);
}

bool SId_3d_6(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (cont_[index_ - si_ - sj_ + sk_] == value && cont_[index_ - sj_] == value &&
          cont_[index_ + si_ - sj_ - sk_] == value &&
          cont_[index_ - si_ + sk_] == value && cont_[index_] == value &&
          cont_[index_ + si_ - sk_] == value &&
          cont_[index_ - si_ + sj_ + sk_] == value && cont_[index_ + sj_] == value &&
          cont_[index_ + si_ + sj_ - sk_] == value);
}

// diagonals loop
bool SId_3d_7(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (cont_[index_ - si_ - sj_ - sk_] == value && cont_[index_ - sk_] == value &&
          cont_[index_ + si_ + sj_ - sk_] == value &&
          cont_[index_ - si_ - sj_] == value && cont_[index_] == value &&
          cont_[index_ + si_ + sj_] == value &&
          cont_[index_ - si_ - sj_ + sk_] == value && cont_[index_ + sk_] == value &&
          cont_[index_ + si_ + sj_ + sk_] == value);
}

bool SId_3d_8(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (cont_[index_ - si_ + sj_ - sk_] == value && cont_[index_ - sk_] == value &&
          cont_[index_ + si_ - sj_ - sk_] == value &&
          cont_[index_ - si_ + sj_] == value && cont_[index_] == value &&
          cont_[index_ + si_ - sj_] == value &&
          cont_[index_ - si_ + sj_ + sk_] == value && cont_[index_ + sk_] == value &&
          cont_[index_ + si_ - sj_ + sk_] == value);
}

bool SId_3d_any(uint8_t const *cont_, int index_, int si_, int sj_, int sk_,
                int value) {
  return (SId_3d_0(cont_, index_, si_, sj_, sk_, value) ||
          SId_3d_1(cont_, index_, si_, sj_, sk_, value) ||
          SId_3d_2(cont_, index_, si_, sj_, sk_, value) ||
          SId_3d_3(cont_, index_, si_, sj_, sk_, value) ||
          SId_3d_4(cont_, index_, si_, sj_, sk_, value) ||
          SId_3d_5(cont_, index_, si_, sj_, sk_, value) ||
          SId_3d_6(cont_, index_, si_, sj_, sk_, value) ||
          SId_3d_7(cont_, index_, si_, sj_, sk_, value) ||
          SId_3d_8(cont_, index_, si_, sj_, sk_, value));
}

// flat surfaces
bool ISd_3d_0(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (!(cont_[index_ - sj_ - sk_] == value || cont_[index_ - sj_] == value ||
            cont_[index_ - sj_ + sk_] == value || cont_[index_ - sk_] == value ||
            cont_[index_] == value || cont_[index_ + sk_] == value ||
            cont_[index_ + sj_ - sk_] == value || cont_[index_ + sj_] == value ||
            cont_[index_ + sj_ + sk_] == value));
}
bool ISd_3d_1(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (!(cont_[index_ - si_ - sk_] == value || cont_[index_ - si_] == value ||
            cont_[index_ - si_ + sk_] == value || cont_[index_ - sk_] == value ||
            cont_[index_] == value || cont_[index_ + sk_] == value ||
            cont_[index_ + si_ - sk_] == value || cont_[index_ + si_] == value ||
            cont_[index_ + si_ + sk_] == value));
}
bool ISd_3d_2(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (!(cont_[index_ - si_ - sj_] == value || cont_[index_ - si_] == value ||
            cont_[index_ - si_ + sj_] == value || cont_[index_ - sj_] == value ||
            cont_[index_] == value || cont_[index_ + sj_] == value ||
            cont_[index_ + si_ - sj_] == value || cont_[index_ + si_] == value ||
            cont_[index_ + si_ + sj_] == value));
}

// diagonals loop i
bool ISd_3d_3(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (!(cont_[index_ - si_ - sj_ - sk_] == value || cont_[index_ - si_] == value ||
            cont_[index_ - si_ + sj_ + sk_] == value ||
            cont_[index_ - sj_ - sk_] == value || cont_[index_] == value ||
            cont_[index_ + sj_ + sk_] == value ||
            cont_[index_ + si_ - sj_ - sk_] == value || cont_[index_ + si_] == value ||
            cont_[index_ + si_ + sj_ + sk_] == value));
}

bool ISd_3d_4(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (!(cont_[index_ - si_ - sj_ + sk_] == value || cont_[index_ - si_] == value ||
            cont_[index_ - si_ + sj_ - sk_] == value ||
            cont_[index_ - sj_ + sk_] == value || cont_[index_] == value ||
            cont_[index_ + sj_ - sk_] == value ||
            cont_[index_ + si_ - sj_ + sk_] == value || cont_[index_ + si_] == value ||
            cont_[index_ + si_ + sj_ - sk_] == value));
}

// diagonals loop j
bool ISd_3d_5(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (!(cont_[index_ - si_ - sj_ - sk_] == value || cont_[index_ - sj_] == value ||
            cont_[index_ + si_ - sj_ + sk_] == value ||
            cont_[index_ - si_ - sk_] == value || cont_[index_] == value ||
            cont_[index_ + si_ + sk_] == value ||
            cont_[index_ - si_ + sj_ - sk_] == value || cont_[index_ + sj_] == value ||
            cont_[index_ + si_ + sj_ + sk_] == value));
}

bool ISd_3d_6(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (!(cont_[index_ - si_ - sj_ + sk_] == value || cont_[index_ - sj_] == value ||
            cont_[index_ + si_ - sj_ - sk_] == value ||
            cont_[index_ - si_ + sk_] == value || cont_[index_] == value ||
            cont_[index_ + si_ - sk_] == value ||
            cont_[index_ - si_ + sj_ + sk_] == value || cont_[index_ + sj_] == value ||
            cont_[index_ + si_ + sj_ - sk_] == value));
}

// diagonals loop k
bool ISd_3d_7(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (!(cont_[index_ - si_ - sj_ - sk_] == value || cont_[index_ - sk_] == value ||
            cont_[index_ + si_ + sj_ - sk_] == value ||
            cont_[index_ - si_ - sj_] == value || cont_[index_] == value ||
            cont_[index_ + si_ + sj_] == value ||
            cont_[index_ - si_ - sj_ + sk_] == value || cont_[index_ + sk_] == value ||
            cont_[index_ + si_ + sj_ + sk_] == value));
}

bool ISd_3d_8(uint8_t const *cont_, int index_, int si_, int sj_, int sk_, int value) {
  return (!(cont_[index_ - si_ + sj_ - sk_] == value || cont_[index_ - sk_] == value ||
            cont_[index_ + si_ - sj_ - sk_] == value ||
            cont_[index_ - si_ + sj_] == value || cont_[index_] == value ||
            cont_[index_ + si_ - sj_] == value ||
            cont_[index_ - si_ + sj_ + sk_] == value || cont_[index_ + sk_] == value ||
            cont_[index_ + si_ - sj_ + sk_] == value));
}

bool ISd_3d_any(uint8_t const *cont_, int index_, int si_, int sj_, int sk_,
                int value) {
  return (!(ISd_3d_0(cont_, index_, si_, sj_, sk_, value) ||
            ISd_3d_1(cont_, index_, si_, sj_, sk_, value) ||
            ISd_3d_2(cont_, index_, si_, sj_, sk_, value) ||
            ISd_3d_3(cont_, index_, si_, sj_, sk_, value) ||
            ISd_3d_4(cont_, index_, si_, sj_, sk_, value) ||
            ISd_3d_5(cont_, index_, si_, sj_, sk_, value) ||
            ISd_3d_6(cont_, index_, si_, sj_, sk_, value) ||
            ISd_3d_7(cont_, index_, si_, sj_, sk_, value) ||
            ISd_3d_8(cont_, index_, si_, sj_, sk_, value)));
}
