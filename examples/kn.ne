function k_parmi_n(k, n) do
    if (k == 0 or n == 0 or n == k) then
        return (1)
    else
        return (k_parmi_n(k-1, n-1) + k_parmi_n(k, n-1))
    end
end
