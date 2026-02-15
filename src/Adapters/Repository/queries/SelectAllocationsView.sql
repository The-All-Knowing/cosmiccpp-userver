SELECT sku, batchref 
FROM allocation.allocations_view 
WHERE orderid = $1;