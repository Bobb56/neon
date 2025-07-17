def k_parmi_n(k, n) :
    if (k == 0 or n == 0 or n == k) :
        return 1
    else:
        return k_parmi_n(k-1, n-1) + k_parmi_n(k, n-1)
