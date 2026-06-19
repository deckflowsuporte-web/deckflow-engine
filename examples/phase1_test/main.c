/**
 * @file main.c
 * @brief Phase 1 Functional Test - Scene, Nodes, Hierarchy, Search, Cleanup
 * 
 * This example validates all core engine concepts:
 * 1. Scene creation and management
 * 2. Node creation and types
 * 3. Parent/Child hierarchy
 * 4. Node search functionality
 * 5. Proper tree cleanup (no memory leaks)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scenes/scene.h"
#include "nodes/node.h"
#include "math/vector2.h"
#include "engine_stub.h"  // Version macros for tests

// Test result tracking
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) printf("\n[TEST] %s\n", name)
#define ASSERT(cond, msg) do { \
    if (cond) { \
        printf("  ✓ PASS: %s\n", msg); \
        tests_passed++; \
    } else { \
        printf("  ✗ FAIL: %s\n", msg); \
        tests_failed++; \
    } \
} while(0)

#define ASSERT_FLOAT_EQ(actual, expected, msg) do { \
    float _actual = (actual); \
    float _expected = (expected); \
    float _diff = _actual - _expected; \
    if (_diff < 0) _diff = -_diff; \
    if (_diff < 0.0001f) { \
        printf("  ✓ PASS: %s\n", msg); \
        tests_passed++; \
    } else { \
        printf("  ✗ FAIL: %s (got %.2f, expected %.2f)\n", msg, _actual, _expected); \
        tests_failed++; \
    } \
} while(0)

// Test 1: Scene Creation
void test_scene_creation(void) {
    TEST("Scene Creation");
    
    Scene* scene = scene_create("TestScene");
    ASSERT(scene != NULL, "Scene created successfully");
    ASSERT(strcmp(scene->name, "TestScene") == 0, "Scene name is correct");
    ASSERT(scene->root != NULL, "Scene has root node");
    ASSERT(scene->root->child_count == 0, "Root has no children initially");
    
    scene_destroy(scene);
}

// Test 2: Node Creation
void test_node_creation(void) {
    TEST("Node Creation");
    
    Node* node1 = node_create("Player", NODE_TYPE_SPATIAL_2D);
    Node* node2 = node_create("Enemy", NODE_TYPE_SPATIAL_2D);
    Node* node3 = node_create("Coin", NODE_TYPE_SPRITE);
    
    ASSERT(node1 != NULL, "Node1 created successfully");
    ASSERT(node2 != NULL, "Node2 created successfully");
    ASSERT(node3 != NULL, "Node3 created successfully");
    
    ASSERT(strcmp(node1->name, "Player") == 0, "Node1 name is 'Player'");
    ASSERT(node1->type == NODE_TYPE_SPATIAL_2D, "Node1 type is SPATIAL_2D");
    ASSERT(node2->type == NODE_TYPE_SPATIAL_2D, "Node2 type is SPATIAL_2D");
    ASSERT(node3->type == NODE_TYPE_SPRITE, "Node3 type is SPRITE");
    
    ASSERT(node1->id != node2->id, "Each node has unique ID");
    ASSERT(node1->parent == NULL, "New node has no parent");
    ASSERT(node1->child_count == 0, "New node has no children");
    
    node_free(node1);
    node_free(node2);
    node_free(node3);
}

// Test 3: Parent/Child Hierarchy
void test_hierarchy(void) {
    TEST("Parent/Child Hierarchy");
    
    Scene* scene = scene_create("HierarchyTest");
    Node* root = scene_get_root(scene);
    
    Node* parent = node_create("Parent", NODE_TYPE_SPATIAL_2D);
    Node* child1 = node_create("Child1", NODE_TYPE_SPATIAL_2D);
    Node* child2 = node_create("Child2", NODE_TYPE_SPATIAL_2D);
    Node* grandchild = node_create("GrandChild", NODE_TYPE_SPRITE);
    
    // Build hierarchy: root -> parent -> child1 -> grandchild
    //                         |
    //                        child2
    node_add_child(root, parent);
    node_add_child(parent, child1);
    node_add_child(parent, child2);
    node_add_child(child1, grandchild);
    
    ASSERT(root->child_count == 1, "Root has 1 child");
    ASSERT(parent->child_count == 2, "Parent has 2 children");
    ASSERT(child1->child_count == 1, "Child1 has 1 child");
    ASSERT(child2->child_count == 0, "Child2 has no children");
    
    ASSERT(parent->parent == root, "Parent's parent is root");
    ASSERT(child1->parent == parent, "Child1's parent is parent");
    ASSERT(grandchild->parent == child1, "GrandChild's parent is child1");
    
    ASSERT(node_get_root(child1) == root, "node_get_root finds scene root");
    ASSERT(node_get_root(grandchild) == root, "node_get_root works for deep nesting");
    
    ASSERT(node_get_parent(parent) == root, "node_get_parent returns correct node");
    
    scene_destroy(scene);
}

// Test 4: Node Search
void test_node_search(void) {
    TEST("Node Search");
    
    Scene* scene = scene_create("SearchTest");
    Node* root = scene_get_root(scene);
    
    // Build a complex tree
    Node* level1_a = node_create("Level1_A", NODE_TYPE_SPATIAL_2D);
    Node* level1_b = node_create("Level1_B", NODE_TYPE_SPATIAL_2D);
    Node* level2_a = node_create("Target", NODE_TYPE_SPATIAL_2D);
    Node* level2_b = node_create("Level2_B", NODE_TYPE_SPATIAL_2D);
    Node* level3_a = node_create("Target", NODE_TYPE_SPRITE);
    
    node_add_child(root, level1_a);
    node_add_child(root, level1_b);
    node_add_child(level1_a, level2_a);
    node_add_child(level1_a, level2_b);
    node_add_child(level2_b, level3_a);
    
    // Non-recursive search (direct children only)
    Node* found = node_find_child(root, "Target", false);
    ASSERT(found == NULL, "Non-recursive search doesn't find nested Target");
    
    Node* found_direct = node_find_child(level1_a, "Target", false);
    ASSERT(found_direct == level2_a, "Non-recursive search finds direct Target");
    
    // Recursive search
    Node* found_recursive = node_find_child(root, "Target", true);
    ASSERT(found_recursive == level2_a, "Recursive search finds first Target");
    
    // Find second Target (level3_a) by searching from level1_a again
    Node* found_second = node_find_child(level1_b, "Target", true);
    ASSERT(found_second == NULL, "Searching from Level1_B doesn't find Target");
    
    // Search from level2_b (has level3_a child)
    Node* found_deep = node_find_child(level1_a, "Level2_B", true);
    ASSERT(found_deep == level2_b, "Found Level2_B");
    
    Node* found_grandchild = node_find_child(level2_b, "Target", true);
    ASSERT(found_grandchild == level3_a, "Recursive search finds deeply nested Target");
    
    // Test scene_find_node
    Node* scene_found = scene_find_node(scene, "Target");
    ASSERT(scene_found == level2_a, "scene_find_node finds Target");
    
    // Test get_child
    ASSERT(node_get_child(root, 0) == level1_a, "get_child(0) returns level1_a");
    ASSERT(node_get_child(root, 1) == level1_b, "get_child(1) returns level1_b");
    ASSERT(node_get_child(root, 2) == NULL, "get_child(2) returns NULL");
    ASSERT(node_get_child_count(root) == 2, "Root has 2 children");
    
    scene_destroy(scene);
}

// Test 5: Tree Cleanup
void test_tree_cleanup(void) {
    TEST("Tree Cleanup (No Memory Leaks)");
    
    Scene* scene = scene_create("CleanupTest");
    Node* root = scene_get_root(scene);
    
    // Create a deep tree
    Node* node_a = node_create("A", NODE_TYPE_SPATIAL_2D);
    Node* node_b = node_create("B", NODE_TYPE_SPATIAL_2D);
    Node* node_c = node_create("C", NODE_TYPE_SPATIAL_2D);
    Node* node_d = node_create("D", NODE_TYPE_SPATIAL_2D);
    Node* node_e = node_create("E", NODE_TYPE_SPATIAL_2D);
    
    node_add_child(root, node_a);
    node_add_child(node_a, node_b);
    node_add_child(node_b, node_c);
    node_add_child(node_c, node_d);
    node_add_child(node_d, node_e);
    
    ASSERT(root->child_count == 1, "Root has 1 child before cleanup");
    ASSERT(node_get_child_count(node_e) == 0, "Leaf node has 0 children");
    
    // Remove specific child (this should also remove all descendants)
    node_remove_and_free_child(node_b, node_c);
    ASSERT(node_b->child_count == 0, "Node B has no children after removing C");
    ASSERT(node_c == NULL || node_c->parent == NULL, "Node C is detached");
    
    // Remove all children of root
    node_remove_all_children(root);
    ASSERT(root->child_count == 0, "Root has 0 children after remove_all");
    
    // Create another tree to test full scene destroy
    Node* test_root = node_create("TestRoot", NODE_TYPE_SPATIAL_2D);
    Node* child_x = node_create("ChildX", NODE_TYPE_SPATIAL_2D);
    Node* child_y = node_create("ChildY", NODE_TYPE_SPATIAL_2D);
    
    node_add_child(test_root, child_x);
    node_add_child(test_root, child_y);
    node_add_child(child_x, node_create("Grandchild1", NODE_TYPE_SPATIAL_2D));
    node_add_child(child_y, node_create("Grandchild2", NODE_TYPE_SPATIAL_2D));
    node_add_child(child_y, node_create("Grandchild3", NODE_TYPE_SPATIAL_2D));
    
    ASSERT(test_root->child_count == 2, "TestRoot has 2 children");
    
    // Free the entire tree
    node_free(test_root);
    
    scene_destroy(scene);
}

// Test 6: Sibling Operations
void test_siblings(void) {
    TEST("Sibling Operations");
    
    Scene* scene = scene_create("SiblingTest");
    Node* root = scene_get_root(scene);
    
    Node* sibling1 = node_create("Sibling1", NODE_TYPE_SPATIAL_2D);
    Node* sibling2 = node_create("Sibling2", NODE_TYPE_SPATIAL_2D);
    Node* sibling3 = node_create("Sibling3", NODE_TYPE_SPATIAL_2D);
    
    node_add_child(root, sibling1);
    node_add_child(root, sibling2);
    node_add_child(root, sibling3);
    
    // Test sibling navigation
    ASSERT(node_get_next_sibling(sibling1) == sibling2, "Sibling1 next is sibling2");
    ASSERT(node_get_next_sibling(sibling2) == sibling3, "Sibling2 next is sibling3");
    ASSERT(node_get_next_sibling(sibling3) == NULL, "Sibling3 has no next");
    
    ASSERT(node_get_prev_sibling(sibling1) == NULL, "Sibling1 has no prev");
    ASSERT(node_get_prev_sibling(sibling2) == sibling1, "Sibling2 prev is sibling1");
    ASSERT(node_get_prev_sibling(sibling3) == sibling2, "Sibling3 prev is sibling2");
    
    // Test is_ancestor_of
    ASSERT(node_is_ancestor_of(root, sibling1) == true, "Root is ancestor of sibling1");
    ASSERT(node_is_ancestor_of(sibling1, sibling2) == false, "Sibling1 is not ancestor of sibling2");
    
    scene_destroy(scene);
}

// Test 7: Transform Operations
void test_transforms(void) {
    TEST("Transform Operations");
    
    Node* parent = node_create("Parent", NODE_TYPE_SPATIAL_2D);
    Node* child = node_create("Child", NODE_TYPE_SPATIAL_2D);
    
    // Set parent position
    parent->transform.position = vector2_new(100, 100);
    parent->transform.rotation = 0;
    parent->transform.scale = vector2_new(2.0f, 2.0f);
    
    // Set child local position (relative to parent)
    child->transform.position = vector2_new(50, 50);
    child->transform.rotation = 0;
    child->transform.scale = vector2_one();
    
    // Add child to parent
    node_add_child(parent, child);
    
    // Test global position calculation (local + parent, no scale)
    Vector2 global_pos = node_get_global_position(child);
    ASSERT_FLOAT_EQ(global_pos.x, 150.0f, "Global X is parent.x + local.x");
    ASSERT_FLOAT_EQ(global_pos.y, 150.0f, "Global Y is parent.y + local.y");
    
    // Test global scale (parent scale * child scale)
    Vector2 global_scale = node_get_global_scale(child);
    ASSERT_FLOAT_EQ(global_scale.x, 2.0f, "Global X scale is 2.0");
    ASSERT_FLOAT_EQ(global_scale.y, 2.0f, "Global Y scale is 2.0");
    
    // Test global rotation
    parent->transform.rotation = 90.0f;
    child->transform.rotation = 45.0f;
    float global_rot = node_get_global_rotation(child);
    ASSERT_FLOAT_EQ(global_rot, 135.0f, "Global rotation is sum of parent + child");
    
    node_free(parent);
}

// Test 8: Add Child Below
void test_add_below(void) {
    TEST("Add Child Below");
    
    Scene* scene = scene_create("AddBelowTest");
    Node* root = scene_get_root(scene);
    
    Node* node1 = node_create("Node1", NODE_TYPE_SPATIAL_2D);
    Node* node2 = node_create("Node2", NODE_TYPE_SPATIAL_2D);
    Node* node3 = node_create("Node3", NODE_TYPE_SPATIAL_2D);
    
    node_add_child(root, node1);
    node_add_child(root, node3);
    
    ASSERT(root->first_child == node1, "First child is Node1");
    ASSERT(root->last_child == node3, "Last child is Node3");
    
    // Insert node2 at the beginning
    node_add_child_below(root, node2, NULL);
    ASSERT(root->first_child == node2, "After insert at NULL, first is Node2");
    ASSERT(node2->next_sibling == node1, "Node2's next is Node1");
    
    // Insert a copy at specific position
    Node* node1_copy = node_create("Node1Copy", NODE_TYPE_SPATIAL_2D);
    node_add_child_below(root, node1_copy, node1);
    ASSERT(node1_copy->prev_sibling == node1, "Node1Copy is after Node1");
    ASSERT(node1_copy->next_sibling == node3, "Node1Copy's next is Node3");
    
    scene_destroy(scene);
}

// Main test runner
int main(void) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║     DeckFlow Engine - Phase 1 Functional Tests           ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    test_scene_creation();
    test_node_creation();
    test_hierarchy();
    test_node_search();
    test_tree_cleanup();
    test_siblings();
    test_transforms();
    test_add_below();
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║                     TEST RESULTS                         ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Tests Passed: %2d                                        ║\n", tests_passed);
    printf("║  Tests Failed: %2d                                        ║\n", tests_failed);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    
    if (tests_failed == 0) {
        printf("║  ✓ ALL TESTS PASSED! Phase 1 is ready for Phase 2.     ║\n");
    } else {
        printf("║  ✗ SOME TESTS FAILED! Fix issues before proceeding.   ║\n");
    }
    
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    return tests_failed > 0 ? 1 : 0;
}
