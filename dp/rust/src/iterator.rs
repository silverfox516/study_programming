/// Iterator Pattern Implementation in Rust
///
/// The Iterator pattern provides a way to access elements of a collection
/// sequentially without exposing its underlying representation. Rust has
/// excellent built-in iterator support, but we'll implement custom iterators
/// to demonstrate the pattern.

use std::collections::HashMap;

/// Custom collection with iterator
pub struct NumberSequence {
    numbers: Vec<i32>,
}

impl NumberSequence {
    pub fn new() -> Self {
        NumberSequence {
            numbers: Vec::new(),
        }
    }

    pub fn add(&mut self, number: i32) {
        self.numbers.push(number);
    }

    pub fn len(&self) -> usize {
        self.numbers.len()
    }

    pub fn is_empty(&self) -> bool {
        self.numbers.is_empty()
    }

    // Create different types of iterators
    pub fn iter(&self) -> NumberIterator {
        NumberIterator::new(&self.numbers)
    }

    pub fn iter_even(&self) -> EvenNumberIterator {
        EvenNumberIterator::new(&self.numbers)
    }

    pub fn iter_reverse(&self) -> ReverseNumberIterator {
        ReverseNumberIterator::new(&self.numbers)
    }

    pub fn iter_range(&self, start: usize, end: usize) -> RangeNumberIterator {
        RangeNumberIterator::new(&self.numbers, start, end)
    }
}

impl Default for NumberSequence {
    fn default() -> Self {
        Self::new()
    }
}

/// Forward iterator
pub struct NumberIterator<'a> {
    numbers: &'a [i32],
    position: usize,
}

impl<'a> NumberIterator<'a> {
    fn new(numbers: &'a [i32]) -> Self {
        NumberIterator {
            numbers,
            position: 0,
        }
    }
}

impl<'a> Iterator for NumberIterator<'a> {
    type Item = &'a i32;

    fn next(&mut self) -> Option<Self::Item> {
        if self.position < self.numbers.len() {
            let result = &self.numbers[self.position];
            self.position += 1;
            Some(result)
        } else {
            None
        }
    }
}

/// Even numbers iterator
pub struct EvenNumberIterator<'a> {
    numbers: &'a [i32],
    position: usize,
}

impl<'a> EvenNumberIterator<'a> {
    fn new(numbers: &'a [i32]) -> Self {
        EvenNumberIterator {
            numbers,
            position: 0,
        }
    }
}

impl<'a> Iterator for EvenNumberIterator<'a> {
    type Item = &'a i32;

    fn next(&mut self) -> Option<Self::Item> {
        while self.position < self.numbers.len() {
            let current = &self.numbers[self.position];
            self.position += 1;
            if *current % 2 == 0 {
                return Some(current);
            }
        }
        None
    }
}

/// Reverse iterator
pub struct ReverseNumberIterator<'a> {
    numbers: &'a [i32],
    position: isize,
}

impl<'a> ReverseNumberIterator<'a> {
    fn new(numbers: &'a [i32]) -> Self {
        ReverseNumberIterator {
            numbers,
            position: numbers.len() as isize - 1,
        }
    }
}

impl<'a> Iterator for ReverseNumberIterator<'a> {
    type Item = &'a i32;

    fn next(&mut self) -> Option<Self::Item> {
        if self.position >= 0 {
            let result = &self.numbers[self.position as usize];
            self.position -= 1;
            Some(result)
        } else {
            None
        }
    }
}

/// Range iterator
pub struct RangeNumberIterator<'a> {
    numbers: &'a [i32],
    current: usize,
    end: usize,
}

impl<'a> RangeNumberIterator<'a> {
    fn new(numbers: &'a [i32], start: usize, end: usize) -> Self {
        let end = end.min(numbers.len());
        let start = start.min(end);

        RangeNumberIterator {
            numbers,
            current: start,
            end,
        }
    }
}

impl<'a> Iterator for RangeNumberIterator<'a> {
    type Item = &'a i32;

    fn next(&mut self) -> Option<Self::Item> {
        if self.current < self.end {
            let result = &self.numbers[self.current];
            self.current += 1;
            Some(result)
        } else {
            None
        }
    }
}

/// Binary tree with iterator
#[derive(Debug)]
pub struct BinaryTree<T> {
    root: Option<Box<TreeNode<T>>>,
}

#[derive(Debug)]
struct TreeNode<T> {
    value: T,
    left: Option<Box<TreeNode<T>>>,
    right: Option<Box<TreeNode<T>>>,
}

impl<T> BinaryTree<T>
where
    T: PartialOrd + Clone,
{
    pub fn new() -> Self {
        BinaryTree { root: None }
    }

    pub fn insert(&mut self, value: T) {
        self.root = Self::insert_node(self.root.take(), value);
    }

    fn insert_node(node: Option<Box<TreeNode<T>>>, value: T) -> Option<Box<TreeNode<T>>> {
        match node {
            None => Some(Box::new(TreeNode {
                value,
                left: None,
                right: None,
            })),
            Some(mut node) => {
                if value <= node.value {
                    node.left = Self::insert_node(node.left.take(), value);
                } else {
                    node.right = Self::insert_node(node.right.take(), value);
                }
                Some(node)
            }
        }
    }

    pub fn in_order_iter(&self) -> InOrderIterator<T> {
        let mut stack = Vec::new();
        let mut current = self.root.as_ref();

        // Initialize stack for in-order traversal
        while let Some(node) = current {
            stack.push(node.as_ref());
            current = node.left.as_ref();
        }

        InOrderIterator { stack }
    }

    pub fn pre_order_iter(&self) -> PreOrderIterator<T> {
        let mut stack = Vec::new();
        if let Some(ref root) = self.root {
            stack.push(root.as_ref());
        }
        PreOrderIterator { stack }
    }

    pub fn post_order_iter(&self) -> PostOrderIterator<T> {
        let mut values = Vec::new();
        Self::collect_post_order(&self.root, &mut values);
        PostOrderIterator {
            values,
            position: 0,
        }
    }

    fn collect_post_order(node: &Option<Box<TreeNode<T>>>, values: &mut Vec<T>) {
        if let Some(ref node) = node {
            Self::collect_post_order(&node.left, values);
            Self::collect_post_order(&node.right, values);
            values.push(node.value.clone());
        }
    }
}

impl<T> Default for BinaryTree<T>
where
    T: PartialOrd + Clone,
{
    fn default() -> Self {
        Self::new()
    }
}

/// In-order iterator for binary tree
pub struct InOrderIterator<'a, T> {
    stack: Vec<&'a TreeNode<T>>,
}

impl<'a, T> Iterator for InOrderIterator<'a, T>
where
    T: Clone,
{
    type Item = &'a T;

    fn next(&mut self) -> Option<Self::Item> {
        if let Some(node) = self.stack.pop() {
            let result = &node.value;

            // Add right subtree to stack
            let mut current = node.right.as_ref();
            while let Some(node) = current {
                self.stack.push(node.as_ref());
                current = node.left.as_ref();
            }

            Some(result)
        } else {
            None
        }
    }
}

/// Pre-order iterator for binary tree
pub struct PreOrderIterator<'a, T> {
    stack: Vec<&'a TreeNode<T>>,
}

impl<'a, T> Iterator for PreOrderIterator<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<Self::Item> {
        if let Some(node) = self.stack.pop() {
            // Add children to stack (right first, then left for correct order)
            if let Some(ref right) = node.right {
                self.stack.push(right.as_ref());
            }
            if let Some(ref left) = node.left {
                self.stack.push(left.as_ref());
            }

            Some(&node.value)
        } else {
            None
        }
    }
}

/// Post-order iterator for binary tree (using pre-collected values)
pub struct PostOrderIterator<T> {
    values: Vec<T>,
    position: usize,
}

impl<T> Iterator for PostOrderIterator<T>
where
    T: Clone,
{
    type Item = T;

    fn next(&mut self) -> Option<Self::Item> {
        if self.position < self.values.len() {
            let result = self.values[self.position].clone();
            self.position += 1;
            Some(result)
        } else {
            None
        }
    }
}

/// Matrix with row and column iterators
pub struct Matrix<T> {
    data: Vec<Vec<T>>,
    rows: usize,
    cols: usize,
}

impl<T> Matrix<T>
where
    T: Clone + Default,
{
    pub fn new(rows: usize, cols: usize) -> Self {
        let mut data = Vec::with_capacity(rows);
        for _ in 0..rows {
            data.push(vec![T::default(); cols]);
        }

        Matrix { data, rows, cols }
    }

    pub fn set(&mut self, row: usize, col: usize, value: T) -> Result<(), String> {
        if row >= self.rows || col >= self.cols {
            return Err("Index out of bounds".to_string());
        }
        self.data[row][col] = value;
        Ok(())
    }

    pub fn get(&self, row: usize, col: usize) -> Option<&T> {
        self.data.get(row)?.get(col)
    }

    pub fn rows(&self) -> usize {
        self.rows
    }

    pub fn cols(&self) -> usize {
        self.cols
    }

    pub fn row_iter(&self, row: usize) -> Option<RowIterator<T>> {
        if row < self.rows {
            Some(RowIterator::new(&self.data[row]))
        } else {
            None
        }
    }

    pub fn col_iter(&self, col: usize) -> Option<ColIterator<T>> {
        if col < self.cols {
            Some(ColIterator::new(&self.data, col))
        } else {
            None
        }
    }

    pub fn diagonal_iter(&self) -> DiagonalIterator<T> {
        DiagonalIterator::new(&self.data)
    }

    pub fn all_elements_iter(&self) -> AllElementsIterator<T> {
        AllElementsIterator::new(&self.data)
    }
}

/// Row iterator
pub struct RowIterator<'a, T> {
    row: &'a [T],
    position: usize,
}

impl<'a, T> RowIterator<'a, T> {
    fn new(row: &'a [T]) -> Self {
        RowIterator { row, position: 0 }
    }
}

impl<'a, T> Iterator for RowIterator<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<Self::Item> {
        if self.position < self.row.len() {
            let result = &self.row[self.position];
            self.position += 1;
            Some(result)
        } else {
            None
        }
    }
}

/// Column iterator
pub struct ColIterator<'a, T> {
    data: &'a [Vec<T>],
    col: usize,
    row: usize,
}

impl<'a, T> ColIterator<'a, T> {
    fn new(data: &'a [Vec<T>], col: usize) -> Self {
        ColIterator { data, col, row: 0 }
    }
}

impl<'a, T> Iterator for ColIterator<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<Self::Item> {
        if self.row < self.data.len() {
            let result = &self.data[self.row][self.col];
            self.row += 1;
            Some(result)
        } else {
            None
        }
    }
}

/// Diagonal iterator (main diagonal)
pub struct DiagonalIterator<'a, T> {
    data: &'a [Vec<T>],
    position: usize,
}

impl<'a, T> DiagonalIterator<'a, T> {
    fn new(data: &'a [Vec<T>]) -> Self {
        DiagonalIterator { data, position: 0 }
    }
}

impl<'a, T> Iterator for DiagonalIterator<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<Self::Item> {
        if self.position < self.data.len() && self.position < self.data[self.position].len() {
            let result = &self.data[self.position][self.position];
            self.position += 1;
            Some(result)
        } else {
            None
        }
    }
}

/// All elements iterator (row by row)
pub struct AllElementsIterator<'a, T> {
    data: &'a [Vec<T>],
    row: usize,
    col: usize,
}

impl<'a, T> AllElementsIterator<'a, T> {
    fn new(data: &'a [Vec<T>]) -> Self {
        AllElementsIterator {
            data,
            row: 0,
            col: 0,
        }
    }
}

impl<'a, T> Iterator for AllElementsIterator<'a, T> {
    type Item = (usize, usize, &'a T);

    fn next(&mut self) -> Option<Self::Item> {
        if self.row < self.data.len() {
            if self.col < self.data[self.row].len() {
                let result = (self.row, self.col, &self.data[self.row][self.col]);
                self.col += 1;
                Some(result)
            } else {
                self.row += 1;
                self.col = 0;
                self.next()
            }
        } else {
            None
        }
    }
}

/// Composite iterator that combines multiple iterators
pub struct CompositeIterator<T> {
    iterators: Vec<Box<dyn Iterator<Item = T>>>,
    current_index: usize,
}

impl<T> CompositeIterator<T> {
    pub fn new() -> Self {
        CompositeIterator {
            iterators: Vec::new(),
            current_index: 0,
        }
    }

    pub fn add_iterator(&mut self, iterator: Box<dyn Iterator<Item = T>>) {
        self.iterators.push(iterator);
    }
}

impl<T> Default for CompositeIterator<T> {
    fn default() -> Self {
        Self::new()
    }
}

impl<T> Iterator for CompositeIterator<T> {
    type Item = T;

    fn next(&mut self) -> Option<Self::Item> {
        while self.current_index < self.iterators.len() {
            if let Some(item) = self.iterators[self.current_index].next() {
                return Some(item);
            } else {
                self.current_index += 1;
            }
        }
        None
    }
}

/// Filtering iterator adapter
pub struct FilteringIterator<I, F>
where
    I: Iterator,
    F: Fn(&I::Item) -> bool,
{
    iterator: I,
    predicate: F,
}

impl<I, F> FilteringIterator<I, F>
where
    I: Iterator,
    F: Fn(&I::Item) -> bool,
{
    pub fn new(iterator: I, predicate: F) -> Self {
        FilteringIterator {
            iterator,
            predicate,
        }
    }
}

impl<I, F> Iterator for FilteringIterator<I, F>
where
    I: Iterator,
    F: Fn(&I::Item) -> bool,
{
    type Item = I::Item;

    fn next(&mut self) -> Option<Self::Item> {
        while let Some(item) = self.iterator.next() {
            if (self.predicate)(&item) {
                return Some(item);
            }
        }
        None
    }
}

/// Grouping iterator that groups consecutive equal elements
pub struct GroupingIterator<I>
where
    I: Iterator,
    I::Item: PartialEq + Clone,
{
    iterator: I,
    current_group: Vec<I::Item>,
    next_item: Option<I::Item>,
}

impl<I> GroupingIterator<I>
where
    I: Iterator,
    I::Item: PartialEq + Clone,
{
    pub fn new(mut iterator: I) -> Self {
        let next_item = iterator.next();
        GroupingIterator {
            iterator,
            current_group: Vec::new(),
            next_item,
        }
    }
}

impl<I> Iterator for GroupingIterator<I>
where
    I: Iterator,
    I::Item: PartialEq + Clone,
{
    type Item = Vec<I::Item>;

    fn next(&mut self) -> Option<Self::Item> {
        if let Some(item) = self.next_item.take() {
            self.current_group.clear();
            self.current_group.push(item.clone());

            // Collect consecutive equal items
            while let Some(next) = self.iterator.next() {
                if next == item {
                    self.current_group.push(next);
                } else {
                    self.next_item = Some(next);
                    break;
                }
            }

            Some(self.current_group.clone())
        } else {
            None
        }
    }
}

/// Utility functions for iterator composition
pub fn chain_iterators<T>(
    iter1: impl Iterator<Item = T> + 'static,
    iter2: impl Iterator<Item = T> + 'static,
) -> CompositeIterator<T> {
    let mut composite = CompositeIterator::new();
    composite.add_iterator(Box::new(iter1));
    composite.add_iterator(Box::new(iter2));
    composite
}

pub fn filter_iterator<I, F>(iterator: I, predicate: F) -> FilteringIterator<I, F>
where
    I: Iterator,
    F: Fn(&I::Item) -> bool,
{
    FilteringIterator::new(iterator, predicate)
}

pub fn group_consecutive<I>(iterator: I) -> GroupingIterator<I>
where
    I: Iterator,
    I::Item: PartialEq + Clone,
{
    GroupingIterator::new(iterator)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_number_sequence_iterators() {
        let mut seq = NumberSequence::new();
        seq.add(1);
        seq.add(2);
        seq.add(3);
        seq.add(4);

        let forward: Vec<&i32> = seq.iter().collect();
        assert_eq!(forward, vec![&1, &2, &3, &4]);

        let reverse: Vec<&i32> = seq.iter_reverse().collect();
        assert_eq!(reverse, vec![&4, &3, &2, &1]);

        let even: Vec<&i32> = seq.iter_even().collect();
        assert_eq!(even, vec![&2, &4]);

        let range: Vec<&i32> = seq.iter_range(1, 3).collect();
        assert_eq!(range, vec![&2, &3]);
    }

    #[test]
    fn test_binary_tree_iterators() {
        let mut tree = BinaryTree::new();
        tree.insert(5);
        tree.insert(3);
        tree.insert(7);
        tree.insert(1);
        tree.insert(9);

        let in_order: Vec<&i32> = tree.in_order_iter().collect();
        assert_eq!(in_order, vec![&1, &3, &5, &7, &9]);

        let pre_order: Vec<&i32> = tree.pre_order_iter().collect();
        assert_eq!(pre_order, vec![&5, &3, &1, &7, &9]);
    }

    #[test]
    fn test_matrix_iterators() {
        let mut matrix: Matrix<i32> = Matrix::new(3, 3);
        for i in 0..3 {
            for j in 0..3 {
                matrix.set(i, j, (i * 3 + j) as i32).unwrap();
            }
        }

        if let Some(row_iter) = matrix.row_iter(1) {
            let row: Vec<&i32> = row_iter.collect();
            assert_eq!(row, vec![&3, &4, &5]);
        }

        if let Some(col_iter) = matrix.col_iter(1) {
            let col: Vec<&i32> = col_iter.collect();
            assert_eq!(col, vec![&1, &4, &7]);
        }
    }

    #[test]
    fn test_filtering_iterator() {
        let numbers = vec![1, 2, 3, 4, 5, 6];
        let filtered = filter_iterator(numbers.into_iter(), |&x| x % 2 == 0);
        let result: Vec<i32> = filtered.collect();
        assert_eq!(result, vec![2, 4, 6]);
    }

    #[test]
    fn test_grouping_iterator() {
        let data = vec![1, 1, 2, 2, 2, 3, 1, 1];
        let grouped = group_consecutive(data.into_iter());
        let result: Vec<Vec<i32>> = grouped.collect();
        assert_eq!(result, vec![vec![1, 1], vec![2, 2, 2], vec![3], vec![1, 1]]);
    }
}

pub fn demo() {
    println!("=== Iterator Pattern Demo ===");

    // Number Sequence Iterators
    println!("\n1. Custom Number Sequence Iterators:");
    let mut sequence = NumberSequence::new();
    for i in 1..=10 {
        sequence.add(i);
    }

    println!("Original sequence: {:?}", sequence.iter().cloned().collect::<Vec<_>>());
    println!("Even numbers: {:?}", sequence.iter_even().cloned().collect::<Vec<_>>());
    println!("Reverse order: {:?}", sequence.iter_reverse().cloned().collect::<Vec<_>>());
    println!("Range [2..6): {:?}", sequence.iter_range(2, 6).cloned().collect::<Vec<_>>());

    // Binary Tree Iterators
    println!("\n2. Binary Tree Traversal Iterators:");
    let mut tree = BinaryTree::new();
    let values = [50, 30, 70, 20, 40, 60, 80];
    for &val in &values {
        tree.insert(val);
    }

    println!("Tree values inserted: {:?}", values);
    println!("In-order traversal: {:?}", tree.in_order_iter().cloned().collect::<Vec<_>>());
    println!("Pre-order traversal: {:?}", tree.pre_order_iter().cloned().collect::<Vec<_>>());
    println!("Post-order traversal: {:?}", tree.post_order_iter().collect::<Vec<_>>());

    // Matrix Iterators
    println!("\n3. Matrix Iterators:");
    let mut matrix: Matrix<i32> = Matrix::new(4, 4);
    for i in 0..4 {
        for j in 0..4 {
            matrix.set(i, j, (i * 4 + j + 1) as i32).unwrap();
        }
    }

    println!("Matrix 4x4:");
    for row in 0..4 {
        if let Some(row_iter) = matrix.row_iter(row) {
            let row_data: Vec<&i32> = row_iter.collect();
            println!("  Row {}: {:?}", row, row_data);
        }
    }

    println!("Column 2: {:?}",
             matrix.col_iter(2).unwrap().cloned().collect::<Vec<_>>());

    println!("Main diagonal: {:?}",
             matrix.diagonal_iter().cloned().collect::<Vec<_>>());

    println!("All elements with positions:");
    for (row, col, value) in matrix.all_elements_iter().take(8) {
        println!("  ({}, {}) = {}", row, col, value);
    }

    // Composite Iterator
    println!("\n4. Composite Iterator:");
    let iter1 = vec![1, 2, 3].into_iter();
    let iter2 = vec![4, 5, 6].into_iter();
    let chained = chain_iterators(iter1, iter2);
    println!("Chained iterators: {:?}", chained.collect::<Vec<_>>());

    // Filtering Iterator
    println!("\n5. Filtering Iterator:");
    let numbers = vec![1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
    println!("Original numbers: {:?}", numbers);

    let filtered_even = filter_iterator(numbers.iter(), |&&x| x % 2 == 0);
    println!("Even numbers: {:?}", filtered_even.cloned().collect::<Vec<_>>());

    let filtered_gt5 = filter_iterator(numbers.iter(), |&&x| x > 5);
    println!("Numbers > 5: {:?}", filtered_gt5.cloned().collect::<Vec<_>>());

    // Grouping Iterator
    println!("\n6. Grouping Iterator:");
    let data = vec![1, 1, 1, 2, 2, 3, 3, 3, 3, 1, 1, 4, 4];
    println!("Original data: {:?}", data);

    let grouped = group_consecutive(data.into_iter());
    let groups: Vec<Vec<i32>> = grouped.collect();
    println!("Grouped consecutive: {:?}", groups);

    // Complex Iterator Chain
    println!("\n7. Complex Iterator Operations:");
    let mut sequence2 = NumberSequence::new();
    for i in 1..=20 {
        sequence2.add(i);
    }

    // Chain multiple operations
    let result: Vec<i32> = sequence2
        .iter()
        .filter(|&&x| x % 3 == 0)  // Multiples of 3
        .map(|&x| x * 2)           // Double them
        .filter(|&x| x < 25)       // Less than 25
        .collect();

    println!("Numbers divisible by 3, doubled, and < 25: {:?}", result);

    // Statistical operations using iterators
    println!("\n8. Statistical Operations:");
    let stats_data = vec![1, 5, 3, 9, 2, 8, 4, 7, 6];
    println!("Data: {:?}", stats_data);

    let sum: i32 = stats_data.iter().sum();
    let count = stats_data.len();
    let avg = sum as f32 / count as f32;
    let max = stats_data.iter().max().unwrap();
    let min = stats_data.iter().min().unwrap();

    println!("Sum: {}, Count: {}, Average: {:.2}", sum, count, avg);
    println!("Max: {}, Min: {}", max, min);

    // Demonstrate lazy evaluation
    println!("\n9. Lazy Evaluation:");
    let lazy_iter = (1..1000000)
        .filter(|&x| x % 2 == 0)
        .map(|x| x * x)
        .filter(|&x| x > 100);

    println!("Created lazy iterator for large dataset");

    let first_five: Vec<i32> = lazy_iter.take(5).collect();
    println!("First 5 results: {:?}", first_five);

    println!("\n10. Iterator Pattern Benefits:");
    println!("  ✅ Provides uniform way to traverse collections");
    println!("  ✅ Decouples algorithms from data structures");
    println!("  ✅ Supports lazy evaluation for efficiency");
    println!("  ✅ Enables functional programming patterns");
    println!("  ✅ Composable and chainable operations");
    println!("  ✅ Memory efficient for large datasets");
    println!("  ✅ Supports different traversal strategies");
}

pub fn main() {
    demo();
}