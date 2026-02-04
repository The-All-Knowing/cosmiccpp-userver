SELECT
    id,
    reference,
    sku,
    _purchased_quantity,
    eta
FROM allocation.batches
WHERE sku = $1;