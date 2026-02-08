DELETE FROM allocation.allocations_view
WHERE orderid = $1 AND sku = $2